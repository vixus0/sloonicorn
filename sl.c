#include <lo/lo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl.h"

sl_t sl = {
  .live = 0,
  .loop_count = 0,
  .loops = { {}, {}, {}, {} }
};

lo_server_thread st;
lo_address ad;

int ping_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  if (sl.live == 0) {
    sl.live = 1;
    sl.loop_count = argv[2]->i % MAX_LOOPS;
    fprintf(stderr, "sloo: pong! %s %s %d\n", &argv[0]->s, &argv[1]->s, sl.loop_count);
  }
  return 0;
}

int selected_loop_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  int rv = (int)(argv[2]->f);
  int loop = (rv < sl.loop_count) ? rv : sl.loop_count - 1;
  sl.loops[loop].selected = 1;
  fprintf(stderr, "looper: selected_loop = %d\n", loop);
  return 0;
}

int loop_len_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].len = argv[2]->f;
  return 0;
}

int loop_pos_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].pos = argv[2]->f;
  return 0;
}

int loop_in_peak_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].in_peak = argv[2]->f;
  return 0;
}

int loop_out_peak_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].out_peak = argv[2]->f;
  return 0;
}

int loop_state_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].state = (int)argv[2]->f;
  fprintf(stderr, "looper: loop[%d] state = %d\n", argv[0]->i, (int)argv[2]->f);
  return 0;
}

int loop_solo_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].solo = (int)argv[2]->f;
  fprintf(stderr, "looper: loop[%d] solo = %d\n", argv[0]->i, (int)argv[2]->f);
  return 0;
}

int loop_waiting_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].waiting = (int)argv[2]->f;
  fprintf(stderr, "looper: loop[%d] waiting = %d\n", argv[0]->i, (int)argv[2]->f);
  return 0;
}

void error_handler(int errno, const char *msg, const char *path) {
  fprintf(stderr, "liblo: error creating server [%d] (%s) %s\n", errno, path, msg);
  fflush(stderr);
}

int sl_init(const char *port, const char *sl_url) {
  st = lo_server_thread_new(port, error_handler);
  ad = lo_address_new_from_url(sl_url);

  if (!st) {
    fprintf(stderr, "sloo: error creating server on port %s\n", port);
    return -1;
  }

  if (lo_address_errno(ad) < 0) {
    fprintf(stderr, "sloo: error connecting to SooperLooper at %s\n", sl_url);
    return -1;
  }

  fprintf(stderr, "sloo: listening on %s\n", lo_server_thread_get_url(st));

  lo_server_thread_add_method(st, "/ping", "ssi", ping_handler, NULL);

  lo_server_thread_add_method(st, "/selected_loop", "isf", selected_loop_handler, NULL);
  lo_server_thread_add_method(st, "/state", "isf", loop_state_handler, NULL);
  lo_server_thread_add_method(st, "/solo", "isf", loop_solo_handler, NULL);
  lo_server_thread_add_method(st, "/waiting", "isf", loop_waiting_handler, NULL);
  lo_server_thread_add_method(st, "/len", "isf", loop_len_handler, NULL);
  lo_server_thread_add_method(st, "/pos", "isf", loop_pos_handler, NULL);
  lo_server_thread_add_method(st, "/in_peak", "isf", loop_in_peak_handler, NULL);
  lo_server_thread_add_method(st, "/out_peak", "isf", loop_out_peak_handler, NULL);

  lo_server_thread_start(st);

  return 0;
}

void sl_end() {
  fprintf(stderr, "sloo: sl_cleanup\n");

  if (ad != NULL && sl_live()) {
    sl_register(1);
  }

  if (st != NULL) {
    lo_server_thread_free(st);
  }
}

void sl_ping() {
  lo_send(ad, "/ping", "ss", lo_server_thread_get_url(st), "/ping");
}

void sl_register(int unreg) {
  const char *url = lo_server_thread_get_url(st);
  int i;

  for (i = 0; i < sl.loop_count; i++) {
    sl_register_loop(i, unreg);
  }

  if (unreg == 0) {
    fprintf(stderr, "sloo: registering callbacks\n");
    lo_send(ad, "/register_update", "sss", "selected_loop_num", url, "/selected_loop");
  } else {
    fprintf(stderr, "sloo: removing callbacks\n");
    lo_send(ad, "/unregister_update", "sss", "selected_loop_num", url, "/selected_loop");
  }
}

void sl_register_loop(int id, int unreg) {
  const char *url = lo_server_thread_get_url(st);
  char buf[128];

  if (unreg == 0) {
    snprintf(buf, sizeof(buf), "/sl/%d/register_auto_update", id);
    lo_send(ad, buf, "siss", "state", UPDATE_MS, url, "/state");
    lo_send(ad, buf, "siss", "is_soloed", UPDATE_MS, url, "/solo");
    lo_send(ad, buf, "siss", "waiting", UPDATE_MS, url, "/waiting");
    lo_send(ad, buf, "siss", "loop_len", UPDATE_MS, url, "/len");
    lo_send(ad, buf, "siss", "loop_pos", UPDATE_MS, url, "/pos");
    lo_send(ad, buf, "siss", "in_peak_meter", UPDATE_MS, url, "/in_peak");
    lo_send(ad, buf, "siss", "out_peak_meter", UPDATE_MS, url, "/out_peak");
  } else {
    snprintf(buf, sizeof(buf), "/sl/%d/unregister_auto_update", id);
    lo_send(ad, buf, "sss", "state", url, "/state");
    lo_send(ad, buf, "sss", "is_soloed", url, "/solo");
    lo_send(ad, buf, "sss", "waiting", url, "/waiting");
    lo_send(ad, buf, "sss", "loop_len", url, "/len");
    lo_send(ad, buf, "sss", "loop_pos", url, "/pos");
    lo_send(ad, buf, "sss", "in_peak_meter", url, "/in_peak");
    lo_send(ad, buf, "sss", "out_peak_meter", url, "/out_peak");
  }
}

int sl_live() {
  return sl.live;
}

void sl_die() {
  sl.live = 0;
}

int sl_loop_count() {
  return sl.loop_count;
}

int sl_loop_state(int id) {
  return sl.loops[id].state;
}

int sl_loop_solo(int id) {
  return sl.loops[id].solo;
}

int sl_loop_selected(int id) {
  return sl.loops[id].selected;
}

int sl_loop_waiting(int id) {
  return sl.loops[id].waiting;
}

float sl_loop_progress(int id) {
  return sl.loops[id].pos / sl.loops[id].len;
}

float sl_loop_in_peak(int id) {
  return sl.loops[id].in_peak;
}

float sl_loop_out_peak(int id) {
  return sl.loops[id].out_peak;
}

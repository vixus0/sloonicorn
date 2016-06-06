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
  return 0;
}

int loop_len_hander(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].len = argv[2]->f;
  return 0;
}

int loop_pos_hander(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].pos = argv[2]->f;
  return 0;
}

int loop_peak_hander(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].in_peak = argv[2]->f;
  return 0;
}

int loop_state_hander(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].state = (int)argv[2]->f;
  return 0;
}

int loop_solo_hander(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].solo = (int)argv[2]->f;
  return 0;
}

int loop_waiting_hander(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
  sl.loops[argv[0]->i].waiting = (int)argv[2]->f;
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

  lo_server_thread_add_method(st, "/state", "isf", loop_state_hander, NULL);
  lo_server_thread_add_method(st, "/solo", "isf", loop_solo_hander, NULL);
  lo_server_thread_add_method(st, "/waiting", "isf", loop_waiting_hander, NULL);

  lo_server_thread_add_method(st, "/len", "isf", loop_len_hander, NULL);
  lo_server_thread_add_method(st, "/pos", "isf", loop_pos_hander, NULL);
  lo_server_thread_add_method(st, "/peak", "isf", loop_peak_hander, NULL);

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
    lo_send(ad, "/register_update", "sss", "selected_loop_num", url, "/selected_loop");
  } else {
    lo_send(ad, "/unregister_update", "sss", "selected_loop_num", url, "/selected_loop");
  }
}

void sl_register_loop(int id, int unreg) {
  const char *url = lo_server_thread_get_url(st);
  char buf[128];

  if (unreg == 0) {
    snprintf(buf, sizeof(buf), "/sl/%d/register_update", id);
  } else {
    snprintf(buf, sizeof(buf), "/sl/%d/unregister_update", id);
  }

  lo_send(ad, buf, "sss", "state", url, "/state");
  lo_send(ad, buf, "sss", "is_soloed", url, "/solo");
  lo_send(ad, buf, "sss", "waiting", url, "/waiting");

  if (unreg == 0) {
    snprintf(buf, sizeof(buf), "/sl/%d/register_auto_update", id);
  } else {
    snprintf(buf, sizeof(buf), "/sl/%d/unregister_auto_update", id);
  }

  lo_send(ad, buf, "siss", "loop_len", UPDATE_MS, url, "/len");
  lo_send(ad, buf, "siss", "loop_pos", UPDATE_MS, url, "/pos");
  lo_send(ad, buf, "siss", "in_peak_meter", UPDATE_MS, url, "/peak");
}

int sl_live() {
  return sl.live;
}

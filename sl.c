#include <stdlib.h>
#include <stdio.h>
#include <lo/lo.h>
#include "sl.h"

sl_t sl = {
  .live = 0,
  .loop_count = 0,
  loops[] = { {}, {}, {}, {} }
}

_lo_handler(ping_handler) {
  sl.live = 1;
  sl.loop_count = argv[2]->i;
}

_lo_handler(selected_loop_handler) {
  int rv = (int)(argv[2]->f);
  int loop = (rv < sl.loop_count) ? rv : sl.loop_count - 1;
  sl.loops[loop].selected = 1;
}

_lo_handler(loop_len_hander) {
  sl.loops[argv[0]->i].len = argv[2]->f;
}

_lo_handler(loop_pos_hander) {
  sl.loops[argv[0]->i].pos = argv[2]->f;
}

_lo_handler(loop_peak_hander) {
  sl.loops[argv[0]->i].peak = argv[2]->f;
}

_lo_handler(loop_state_hander) {
  sl.loops[argv[0]->i].state = (int)argv[2]->f;
}

_lo_handler(loop_solo_hander) {
  sl.loops[argv[0]->i].solo = (int)argv[2]->f;
}

void error_handler(int errno, const char *msg, const char *path) {
  fprintf(stderr, "error [%d] (%s) %s\n", errno, path, msg);
  fflush(stderr);
}

int sl_init(const char *sl_url) {
  lo_server_thread st = lo_server_thread_new(SOCK_PATH, error_handler);

  if (!st) {
    return -1;
  }


}

void sl_ping(const char *sl_url) {
   
}

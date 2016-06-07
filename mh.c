#include <lo/lo.h>
#include <stdio.h>
#include "mh.h"
#include "sl.h"

#define led(x, y, clr) lo_send(mh, "/set", "iii", x, y, clr)

lo_address mh;

int mh_init(const char *mh_url) {
  mh = lo_address_new_from_url(mh_url);

  if (lo_address_errno(mh) < 0) {
    fprintf(stderr, "sloo: error connecting to Monohorn at %s\n", mh_url);
    return -1;
  }

  return 0;
}

void mh_update() {
  int loop, i, y, clr, prog, peak;

  for (loop = 0; loop < MAX_LOOPS; loop++) {
    y = loop * ROW_H;

    // progress bar (color indicates recording/playing/muted)
    prog = sl_loop_progress(loop) * ROW_W;

    switch (sl_loop_state(loop)) {
      case 2: // recording
        clr = C_red;
        break;
      case 4: // playing
        clr = C_lightblue;
        break;
      case 5: // overdubbing
        clr = C_purple;
        break;
      case 8: // replacing
        clr = C_pink;
        break;
      default: // muted, everything else
        clr = C_blue;
    }

    if (sl_loop_solo(loop) == 1) clr = C_yellow;

    for (i = 0; i < prog; i++) led(i, y, clr);
    for (; i < ROW_W; i++) led(i, y, C_black);

    // peak indicator
    peak = sl_loop_out_peak(loop) * ROW_W;
    for (i = 0; i < peak; i++) led(i, y+1, C_yellow);
    for (; i < ROW_W; i++) led(i, y+1, C_black);

    // selected indicator
    if (sl_loop_selected(loop) == 1) {
      led(0, y, C_green);
      led(0, y+1, C_green);
    }

    // waiting indicator
    if (sl_loop_waiting(loop) == 1) {
      led(0, y, C_orange);
      led(0, y+1, C_orange);
    }
  }

  lo_send(mh, "/render", NULL);
}

void mh_end() {
  lo_send(mh, "/clear", NULL);
}
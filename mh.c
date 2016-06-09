#include <lo/lo.h>
#include <math.h>
#include <stdio.h>
#include "mh.h"
#include "sl.h"

#define led(x, y, clr) lo_send(mh, "/set", "iii", x, y, clr)

lo_address mh;

int mh_init(const char *mh_url) {
  fprintf(stderr, "mh: connecting to Monohorn at %s\n", mh_url);

  mh = lo_address_new_from_url(mh_url);

  if (lo_address_errno(mh) < 0) {
    fprintf(stderr, "mh error: could not connect to Monohorn at %s\n", mh_url);
    return -1;
  }

  if (lo_send(mh, "/clear", NULL) == -1) {
    fprintf(stderr, "mh error: could not send to Monohorn\n");
    return -1;
  }

  return 0;
}

void mh_update() {
  fprintf(stderr, "mh: Monohorn update\n");

  int loop, i, y, clr, prog, peak;

  for (loop = 0; loop < sl_loop_count(); loop++) {
    y = loop * ROW_H;

    // progress indicator (bg color indicates recording/playing/muted)
    prog = (int)floor(sl_loop_progress(loop) * (float)ROW_W);

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

    // bar background
    for (i = 0; i < ROW_W; i++) led(i, y, clr);

    // progress
    led(prog, y, C_white);

    // peak indicator
    peak = (int)floor(sl_loop_out_peak(loop) * (float)ROW_W);
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

    // logging
    fprintf(stderr, "mh: loop[%d] state = %d, prog = %f, peak = %f, sel = %d, wait = %d\n",
        loop,
        sl_loop_state(loop),
        sl_loop_progress(loop),
        sl_loop_out_peak(loop),
        sl_loop_selected(loop),
        sl_loop_waiting(loop)
        );
  }

  lo_send(mh, "/render", NULL);
}

void mh_end() {
  fprintf(stderr, "mh: Monohorn cleanup\n");
  lo_send(mh, "/clear", NULL);
}

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl.h"

#define msleep(t) usleep(1000 * t)

#define LOCAL_URL "osc.unix://localhost/tmp/sloonicorn.socket"
#define SL_URL "osc.unix://localhost/tmp/sooperlooper.socket"
#define MH_URL "osc.unix://localhost/tmp/monohorn.socket"

const char *local_url, *sl_url, *mh_url;
lo_server_thread st;
lo_address ad;

void handle_sigint(int signal) {
  fprintf(stderr, "sloo: SIGINT exiting\n");
  sl_end(ad, st, local_url);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  char opt;

  signal(SIGINT, handle_sigint);

  local_url = LOCAL_URL;
  sl_url = SL_URL;
  mh_url = MH_URL;

  while ((opt = getopt(argc, argv, "l:s:m:")) != -1) {
    switch(opt) {
      case 'l':
        local_url = optarg;
        break;
      case 's':
        sl_url = optarg;
        break;
      case 'm':
        mh_url = optarg;
        break;
      default:
        fprintf(stderr, "%s [-l local_url] [-s sl_url] [-m mh_url]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if (sl_init(local_url, sl_url) == -1) {
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "sloo: listening on %s\n", local_url);

  while (sl.live == 0) {
    fprintf(stderr, "sloo: pinging SL at %s\n", sl_url);
    msleep(100);
  }

  sl_end(ad, st, local_url);

  return EXIT_SUCCESS;
}

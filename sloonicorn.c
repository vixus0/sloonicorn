#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl.h"
#include "mh.h"

#define msleep(t) usleep(1000 * t)

#define LOCAL_PORT "9955"
#define SL_URL "osc.udp://localhost:9951"
#define MH_URL "osc.unix://localhost/tmp/monohorn.socket"

const char *local_port, *sl_url, *mh_url;

void handle_sigint(int signal) {
  fprintf(stderr, "sloo: SIGINT exiting\n");
  sl_end();
  mh_end();
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  char opt;

  fprintf(stdout, "Sloooooooooooooonicorn!\n\n");
  signal(SIGINT, handle_sigint);

  local_port = LOCAL_PORT;
  sl_url = SL_URL;
  mh_url = MH_URL;

  while ((opt = getopt(argc, argv, "p:s:m:")) != -1) {
    switch(opt) {
      case 'p':
        local_port = optarg;
        break;
      case 's':
        sl_url = optarg;
        break;
      case 'm':
        mh_url = optarg;
        break;
      default:
        fprintf(stderr, "%s [-p port] [-s sl_url] [-m mh_url]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if (sl_init(local_port, sl_url) == -1) {
    fprintf(stderr, "Failed to init OSC server\n");
    exit(EXIT_FAILURE);
  }

  if (mh_init(mh_url) == -1) {
    fprintf(stderr, "Failed to contact monohorn\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    while (sl_live() == 0) {
      fprintf(stderr, "sloo: pinging SL at %s\n", sl_url);
      sl_ping();
      msleep(1000);
    }

    sl_register(0);
    sl_die();
    mh_update();
    msleep(100);
  }

  sl_end();
  mh_end();

  return EXIT_SUCCESS;
}

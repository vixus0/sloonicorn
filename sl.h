//
// sl.h
//
// SooperLooper interface
//

#include <lo/lo.h>

#define MAX_LOOPS 4
#define UPDATE_MS 100
#define len(a) (sizeof(a) / sizeof(a[0]))

typedef struct loop {
  int state, solo, selected, waiting;
  float len, pos, in_peak;
} loop_t;

typedef struct sl {
  int live, loop_count;
  loop_t loops[];
} sl_t;

int ping_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int selected_loop_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

int loop_len_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_pos_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_state_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_peak_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_solo_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_waiting_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

int sl_init(const char *local_url, const char *sl_url);
void sl_end(lo_address ad, lo_server_thread st, const char *local_url);
void sl_ping(lo_address ad, const char *local_url);
void sl_register(lo_address ad, const char *local_url, int unreg);
void sl_register_loop(lo_address ad, const char *local_url, int id, int unreg);

extern sl_t sl;

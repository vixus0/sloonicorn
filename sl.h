//
// sl.h
//
// SooperLooper interface
//

#include <lo/lo.h>

#define MAX_LOOPS 4
#define UPDATE_MS 50
#define len(a) (sizeof(a) / sizeof(a[0]))

typedef struct loop {
  int state, solo, selected, waiting;
  float len, pos, in_peak, out_peak;
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
int loop_in_peak_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_out_peak_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_solo_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
int loop_waiting_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

int sl_live();
int sl_init(const char *port, const char *sl_url);
void sl_end();
void sl_ping();
void sl_init_loops();
void sl_register(int unreg);
void sl_register_loop(int id, int unreg);

int sl_loop_count();
int sl_loop_state(int id);
int sl_loop_solo(int id);
int sl_loop_selected(int id);
int sl_loop_waiting(int id);
float sl_loop_progress(int id);
float sl_loop_in_peak(int id);
float sl_loop_out_peak(int id);

int sl_last_reply();
void sl_die();
void sl_update();

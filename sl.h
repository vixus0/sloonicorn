//
// sl.h
//
// SooperLooper interface
//

#define len(a) sizeof(a) / sizeof(a[0])
#define _lo_handler(f) \
  int f(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data)
#define _unpack_sl sl_t *sl = (sl_t *) user_data

typedef struct loop {
  int state, solo, selected;
  float len, pos, in_peak;
} loop_t;

typedef struct sl {
  int live, loop_count;
  loop_t loops[];
} sl_t;

_lo_handler(ping_handler);
_lo_handler(selected_loop_handler);

_lo_handler(loop_len_handler);
_lo_handler(loop_pos_handler);
_lo_handler(loop_state_handler);
_lo_handler(loop_peak_handler);
_lo_handler(loop_solo_handler);

int sl_init(const char *sl_url, sl_t *sl);
void sl_ping(const char *sl_url);

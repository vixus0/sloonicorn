//
// mh.h
//
// Monohorn graphical output
//

#define C_black     0
#define C_red       0x002000
#define C_orange    0x1a2000
#define C_yellow    0x202000
#define C_green     0x200000
#define C_lightblue 0x1a0020
#define C_blue      0x000020
#define C_purple    0x1a1a20
#define C_pink      0x002020
#define C_white     0x202020
#define C_cyan      0x200020
#define C_magenta   0x00201a

#define ROW_H 2
#define ROW_W 8

int mh_init(const char *mh_url);
int mh_clear();
void mh_led(int x, int y, int clr);
void mh_update();
void mh_end();

#ifndef _H_GAME_H_
#define _H_GAME_H_

int Game_printf(const char* fmt, ...);
void Game_init_args(int argc, char** argv);
int Game_loop();
float Game_delta();

int Game_win_get_width();
int Game_win_get_height();

#endif
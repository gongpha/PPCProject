#ifndef _H_GAME_H_
#define _H_GAME_H_

int game__printf(const char* fmt, ...);
void game__init_args(int argc, char** argv);
int game__loop();

#endif
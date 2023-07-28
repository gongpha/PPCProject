#ifndef _H_CONSOLE_H_
#define _H_CONSOLE_H_

int Con_init();
void Con_printf(const char* fmt, ...);

void Con_draw_console();

int Con_resize_console(int width, int height);

void Con_make_showing(bool_t yes);

#endif
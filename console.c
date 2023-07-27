#include "maindef.h"

#define MAX_MESSAGE_LENGTH 256

texture_t console_bg;

int Con_init() {
	Texture_new(&console_bg);
	if (Texture_load_from_file(&console_bg, ASSET("core/console.png")) != ERR_OK) {
		return ERR_CANNOT_LOAD_RESOURCE;
	}
}

// printf but for console
void Con_printf(const char* fmt, ...) // unlimit args hehe
{
	
}

void Con_draw_console()
{
	Texture_draw(&console_bg, 16, 16);
}

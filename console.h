#ifndef _H_CONSOLE_H_
#define _H_CONSOLE_H_

typedef struct concommand_s {
	const char* name;
	void(*run)();

	struct concommand_s* next;
} concommand_t;

int Con_init();
void Con_register_cmd(concommand_t* cmd);
concommand_t* Con_find(const char* cmd_name);
void Con_load_cfg();
void Con_printf(const char* fmt, ...);
void Con_execute(char* line);

void Con_draw_console();

int Con_resize_console(int width, int height);

void Con_make_showing(bool_t yes);

#define CONSOLECMD(name, func) void func(); concommand_t cmd_##name = { #name, func }
#define CMDREGISTER(name) Con_register_cmd(&cmd_##name);

#endif
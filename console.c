#include "maindef.h"

// i hac game

uint32_t console_line_charcters = 0;
uint32_t console_line_count = 0;
MEMSTATIC(console_log); // size = line_charcters * line_count

uint32_t console_line_begin = 0; // top line
uint32_t console_line_cursor = 0; // line for writing

float console_show = 1.0f; // 1.0 = fully show ; 0.0 = hidden
float console_target = 1.0f;

texture_t console_bg;
font_t hasklig;

concommand_t* cmd_root;

int Con_init() {
	cmd_root = NULL;

	Texture_new(&console_bg);
	if (Texture_load_from_file(&console_bg, ASSET("core/console.png")) != ERR_OK) {
		return ERR_CANNOT_LOAD_RESOURCE;
	}

	Font_new_from_file(&hasklig, ASSET("fonts/hasklig.ttf"), 32.0f);

	Con_resize_console(Game_win_get_width(), Game_win_get_height());
}

void Con_register_cmd(concommand_t* cmd)
{
	if (cmd_root == NULL) {
		// ah yes im first
		cmd_root = cmd;
		cmd_root->next = NULL;
	}
	else {
		concommand_t* cursor = cmd_root;
		while (cursor->next) {
			if (String_MATCH(cursor->name, cmd->name)) {
				Con_printf("command \"%s\" is already exist bro", cursor->name);
				return;
			}
			cursor = cursor->next;
		}
		cursor->next = cmd;
	}
}

concommand_t* Con_find(const char* cmd_name) {
	concommand_t* cursor = cmd_root;
	while (cursor) {
		if (String_MATCH(cursor->name, cmd_name))
			return cursor;
		cursor = cursor->next;
	}
	return NULL;
}

// printf but for console
void Con_printf(const char* fmt, ...) // unlimit args hehe
{
	return; // todo : fix this
	va_list arg;
	char lineprint[4096];

	va_start(arg, fmt);
	vsprintf(lineprint, fmt, arg);
	va_end(arg);

	char* cursor = lineprint;
	char* log = (char*)console_log.data;
	uint32_t x = 0;

#define NEWLINE console_line_cursor++;\
	if (x < console_line_charcters) {\
		log[console_line_cursor * console_line_charcters + x] = 0x0;\
	}\
	console_line_cursor %= console_line_count;\
	if (console_line_begin == console_line_cursor) {\
		console_line_begin++;\
		console_line_begin %= console_line_count;\
	}

	while (*cursor) {
		if (*cursor == '\n') {

			NEWLINE;
			x = 0;
			continue;
		}

		log[console_line_cursor * console_line_charcters + x] = *cursor;
		cursor++;
	}
	NEWLINE;
}

void Con_draw_console()
{
	if (console_show <= 0.0) return;

	if (console_target < console_show)
		console_show -= Game_delta();
	else
		console_show += Game_delta();

	int height = Game_win_get_height();
	int y = height * (console_show - 1.0);

	// background
	Texture_set_modulate(defcol_gray);
	Texture_draw_wsize(&console_bg, 0, y, Game_win_get_width(), height);

	Font_draw_cstring(&hasklig, "concon :)\nxd", 16, 64 + y, 1, 1, defcol_white);
}

int Con_resize_console(int width, int height) {
	uint32_t new_line_charcters = width / Font_get_line_height(&hasklig);
	uint32_t new_line_count = height / 48.0f;

	MEMSTATIC(new_console_log);
	if (Mem_create_array(&new_console_log, sizeof(char), new_line_charcters * new_line_count) == ERR_OUT_OF_MEMORY) {
		return ERR_OUT_OF_MEMORY;
	}
	char* newlog = new_console_log.data; // new_line_charcters * new_line_count
	char* oldlog = console_log.data; // line_charcters * line_count

	if (oldlog) {
		uint32_t min_line_count = new_line_count < console_line_count ? new_line_count : console_line_count;
		uint32_t min_line_charcters = new_line_charcters < console_line_charcters ? new_line_charcters : console_line_charcters;

		// copy each lines to new mem
		uint32_t x, y;
		for (y = 0; y < min_line_count; y++) {
			for (x = 0; x < min_line_charcters; x++) {
				uint32_t from_line = (console_line_begin + y) % min_line_count;

				newlog[y * new_line_charcters + x] = oldlog[from_line * console_line_charcters + x];
			}
			if (x < min_line_charcters) {
				newlog[x] = 0x00;
			}
		}
		if (y < min_line_count) {
			newlog[y * new_line_charcters] = 0x00;
		}

		console_line_begin = 0;
		if (console_line_count)
			console_line_cursor = (console_line_cursor - console_line_count) % console_line_count;

		new_line_charcters = console_line_charcters;
		new_line_count = console_line_count;
	}
	Mem_replace(&new_console_log, &console_log);
	return ERR_OK;
}

void Con_make_showing(bool_t yes)
{
	console_target = yes ? 1.0f : 0.0f;
}

void Con_load_cfg() {
	FILE* f = fopen(ASSET("default.cfg"), "r");
	if (!f) return; // dont care

	char line[512];
	while (fgets(line, 512, f)) {
		if (line[0] == '#') continue; // comment

		// execute
		Con_execute(line);
	}
}

void Con_execute(char* line) {
	char* cursor = line;
	char* cmd = line;
	char* arg = NULL;
	int argi = 0;

	while (*cursor) {
		if (*cursor == ' ') {
			*cursor = 0x00;
			arg = cursor + 1;

			// first
			concommand_t* found = Con_find(cmd);
			if (found) {
				// cmd
				(*found->run)();
			}
			else {
				// cvar ?
				cvar_t* found = Cvar_find(cmd);
				if (!found) {
					Con_printf("command or cvar \"%s\" is not exist bro", cmd);
				}
				else {
					// cvar
					Cvar_seto(found, arg);
				}
			}

			break;
		}
		cursor++;

		if (*cursor == '\n') {
			*cursor = 0x00;
			break;
		}
	}

	int a = 0;
}
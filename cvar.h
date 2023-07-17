#ifndef _H_CVAR_H_
#define _H_CVAR_H_

#define CVAR_CREATE(cvar_name, def) static struct cvar_s cvar_name = { .name = #cvar_name, .def_value = def }

typedef struct cvar_s {
	const char* name;
	const char* def_value;

	char* str;
	number_t num;

	// after registered
	struct cvar_t* next;
} cvar_t;

void cvar_register(cvar_t* cvar);

cvar_t* cvar_find(const char* cvar_name);
number_t cvar_read_num(const char* cvar_name, number_t def);

#endif
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

void Cvar_register(cvar_t* cvar);

cvar_t* Cvar_find(const char* cvar_name);
number_t Cvar_read_num(const char* cvar_name, number_t def);

void Cvar_seto(cvar_t* cvar, const char* value);

#endif
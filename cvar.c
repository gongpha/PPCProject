#include "maindef.h"

// linked list
static cvar_t* cvar_root = NULL;

void Cvar_register(cvar_t* cvar) {
	if (cvar_root == NULL) {
		// ah yes im first
		cvar_root = cvar;
		cvar_root->next = NULL;
	}
	else {
		cvar_t* cursor = cvar_root;
		while (cursor->next) {
			if (String_MATCH(cursor->name, cvar->name)) {
				Con_printf("cvar \"%s\" is already exist bro", cursor->name);
				return;
			}
			cursor = cursor->next;
		}
		cursor->next = cvar;
	}

	cvar->str = NULL;
	cvar->num = strtod(cvar->def_value, NULL);
}

cvar_t* Cvar_find(const char* cvar_name) {
	cvar_t* cursor = cvar_root;
	while (cursor) {
		if (String_MATCH(cursor->name, cvar_name))
			return cursor;
		cursor = cursor->next;
	}
	return NULL;
}

number_t Cvar_read_num(const char* cvar_name, number_t def) {
	cvar_t* found = Cvar_find(cvar_name);
	if (!found) return def;

	return found->num;
}

void Cvar_seto(cvar_t* cvar, const char* value)
{
}

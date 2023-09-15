#ifndef _H_WORLD_H_
#define _H_WORLD_H_

#include "maindef.h"

typedef struct {
	const char* key;
	const char* value;
} entitysrc_keyvalue_t;

typedef struct {
	const char* classname;

	// no classname
	entitysrc_keyvalue_t* keyvalues;
	size_t keyvalues_count;
} entitysrc_ent_t;

int entitysrc_parse(const char* entsrc);
void entitysrc_get_player_start_position(vec3 dest);

#endif
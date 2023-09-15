#include "entity_src.h"

// read entity source file

MEMSTATIC(entities);

vec3 player_start_position;

int entitysrc_parse(const char* entsrc)
{
	glm_vec3_zero(player_start_position);

	MEMCREATEDYNAMICARRAY(entities, entitysrc_ent_t);
	MEMSTATIC(keyvalues);
	MEMCREATEDYNAMICARRAY(keyvalues, entitysrc_keyvalue_t);

	size_t ent_num = 0;

	char* cursor = entsrc;
	while (*cursor != '\0') {
		if (*cursor == '{') {
			size_t kv_num = 0;

			char* classname = NULL;

			// parse entity
			cursor++;
			while (*cursor != '}') {
				if (*cursor == '"') {
					// parse key
					cursor++;
					char* key = cursor;
					while (*cursor != '"') {
						cursor++;
					}
					*cursor = '\0';
					cursor++;
					// parse value
					while (*cursor != '"') {
						cursor++;
					}
					cursor++;
					char* value = cursor;
					while (*cursor != '"') {
						cursor++;
					}
					*cursor = '\0';
					cursor++;

					if (!strcmp(key, "classname")) {
						classname = value;
					}
					else {
						entitysrc_keyvalue_t kv;
						kv.key = key;
						kv.value = value;
						MEMDYNAMICARRAYPUSH(keyvalues, entitysrc_keyvalue_t, kv_num, kv);
					}
				}
				cursor++;
			}
			entitysrc_ent_t e;
			e.keyvalues = keyvalues.data;
			e.keyvalues_count = kv_num;
			e.classname = classname;
			classname = NULL;

			MEMDYNAMICARRAYPUSH(entities, entitysrc_ent_t, ent_num, e);
		}
		cursor++;
	}

	for (int i = 0; i < entities.size; i++) {
		entitysrc_ent_t* e = MEMARRAYINDEXPTR(entities, entitysrc_ent_t, i);

		if (strcmp(e->classname, "info_player_start")) continue;

		for (int j = 0; j < e->keyvalues_count; j++) {
			entitysrc_keyvalue_t* kv = &(e->keyvalues[j]);

			if (!strcmp(kv->key, "origin")) {
				sscanf(kv->value, "%f %f %f", &player_start_position[0], &player_start_position[1], &player_start_position[2]);
			}
		}
	}

	return 0;
}

void entitysrc_clear() {
	Mem_release(&entities);
}

void entitysrc_get_player_start_position(vec3 dest)
{
	glm_vec3_copy(player_start_position, dest);
}
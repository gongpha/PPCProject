#ifndef _H_WORLD_H_
#define _H_WORLD_H_

#include "bsp.h"
#include "maindef.h"

#include "glad.h"

int world__load_bsp(const char* map_name);

typedef struct
{
	char name[16];
	u_long size[2];
	GLint texture;
} world_texture_t;

#endif
#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include "maindef.h"
#include "glad.h"

typedef struct {
	GLint program;
} shader_t;

#define SHADER_STATIC(name) static shader_t name = {-1};

void Shader_create(shader_t* shader, const char* vshader, const char* fshader);
void Shader_free(shader_t* shader);
void Shader_use(shader_t* shader);

#endif
#ifndef _H_SOLIDDRAW_H_
#define _H_SOLIDDRAW_H_

// draw shape stuff

#include "maindef.h"

void Draw_update_viewport(float width, float height);


typedef struct {
	memory_t buffer;

	GLuint vao;
	GLuint vbo;
} soliddraw_t;

void Soliddraw_get_shader(shader_t** pptr);
void Soliddraw_create(soliddraw_t* draw);
void Soliddraw_free(soliddraw_t* draw);
void Soliddraw_clear(soliddraw_t* draw);

void Soliddraw_update(soliddraw_t* draw);
void Soliddraw_draw(soliddraw_t* draw);

void Soliddraw_rectangle(soliddraw_t* draw,
	float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4,
	color_t c1, color_t c2, color_t c3, color_t c4
);

#endif
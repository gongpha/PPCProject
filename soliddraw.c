#include "draw.h"
#include <string.h>

#include <cglm/cglm.h>

#include "shader.h"
#include "shaders.h"
#include "soliddraw.h"

static int initialized = 0;
SHADER_STATIC(shader);

static void try_init() {
	if (initialized) return;
	initialized = 1;

	Shader_create(&shader, SOLID_VSHADER, SOLID_FSHADER);
}

void Soliddraw_get_shader(shader_t** pptr)
{
	*pptr = &shader;
}

void Soliddraw_create(soliddraw_t* draw)
{
	MEM(draw->buffer);

	glGenVertexArrays(1, &draw->vao);
	glGenBuffers(1, &draw->vbo);
	glBindVertexArray(draw->vao);
	glBindBuffer(GL_ARRAY_BUFFER, draw->vbo);

	// pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// col
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	try_init();

	// ---
	glBindVertexArray(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Soliddraw_free(soliddraw_t* draw)
{
	glDeleteVertexArrays(1, &draw->vao);
	glDeleteBuffers(1, &draw->vbo);
	Mem_release(&draw->buffer);
}

void Soliddraw_clear(soliddraw_t* draw)
{
	Mem_release(&draw->buffer);
}

void Soliddraw_update(soliddraw_t* draw)
{
	glBindBuffer(GL_ARRAY_BUFFER, draw->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * draw->buffer.size, draw->buffer.data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Soliddraw_draw(soliddraw_t* draw)
{
	Shader_use(&shader);
	glBindVertexArray(draw->vao);
	glDrawArrays(GL_TRIANGLES, 0, draw->buffer.size);
}

void Soliddraw_rectangle(soliddraw_t* draw,
	float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4,
	color_t c1, color_t c2, color_t c3, color_t c4
)
{
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	uint32_t new_start = draw->buffer.size * draw->buffer.size_each;
	float* data = (float*)draw->buffer.data;
	Mem_create_array(&draw->buffer, sizeof(float), draw->buffer.size + 1);


#define V(n)\
	data[new_start] = x##n; new_start++;\
	data[new_start] = y##n; new_start++;\
	data[new_start] = 0.0; new_start++;\
	data[new_start] = c##n[0]; new_start++;\
	data[new_start] = c##n[1]; new_start++;\
	data[new_start] = c##n[2]; new_start++;\
	data[new_start] = c##n[3]; new_start++

	V(4);
	V(2);
	V(1);

	V(4);
	V(3);
	V(2);

#undef V
}
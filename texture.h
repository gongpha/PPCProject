#ifndef _H_TEXTURE_H_
#define _H_TEXTURE_H_

#include "maindef.h"
#include "glad.h"

typedef struct {
	uint32_t width;
	uint32_t height;

	GLint id;
} texture_t;

void Texture_get_shader(shader_t** pptr);
void Texture_new(texture_t* texture);
int Texture_load_from_file(texture_t* texture, const char* path);
int Texture_from_data(texture_t* texture, uint8_t* data, int width, int height, int comp);
void Texture_delete(texture_t* texture);

void Texture_set_modulate(color_t color);
void Texture_reset_modulate();

void Texture_draw_wsize(texture_t* texture,
	float x, float y,
	float xsize, float ysize
);
void Texture_draw(texture_t* texture,
	float x, float y
);

#endif
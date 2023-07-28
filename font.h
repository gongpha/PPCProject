#ifndef _H_FONT_H_
#define _H_FONT_H_

#include "maindef.h"

#include "stb_truetype.h"

typedef struct {
	float height;
	GLuint ftex;
	stbtt_bakedchar characters[128 - 32];

	int asc, des;
	int gap;
} font_t;

void Font_get_shader(shader_t**pptr);
int Font_new_from_file(font_t* font, char* path, float pixel_height);
int Font_new_from_file_multiple(font_t** font, char* path, float* pixel_height, uint32_t count);
int Font_new(font_t* font, uint8_t* font_buffer, float pixel_height);
float Font_get_line_height(font_t* font);
void Font_draw_cstring(font_t* font, const char* str, float x, float y, float scale_x, float scale_y, color_t col);
void Font_free(font_t* font);

#endif
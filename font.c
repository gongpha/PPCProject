#include "maindef.h"
#define FONTTEXTURE_SIZE 512

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static int initialized = 0;
SHADER_STATIC(shader);
static uint32_t VAO, VBO;

const float vertices[] = {
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
};

#include "shaders.h"

static void try_init() {
	if (initialized) return;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	initialized = 1;

	Shader_create(&shader, FONT_VSHADER, FONT_FSHADER);
}

void Font_get_shader(shader_t** pptr)
{
	*pptr = &shader;
}

int Font_new_from_file(font_t* font, char* path, float pixel_height) {
	font_t* fonts[1] = { font };
	float pxls[1] = { pixel_height };
	return Font_new_from_file_multiple(fonts, path, pxls, 1);
}

// import multiple fonts from one file
int Font_new_from_file_multiple(font_t** font, char* path, float* pixel_height, uint32_t count) {
	FILE* f = fopen(path, "rb");
	if (!f) return ERR_CANNOT_LOAD_RESOURCE;
	fseek(f, 0, SEEK_END);
	uint64_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* data = malloc(size);
	if (!data) return ERR_OUT_OF_MEMORY;

	fread(data, size, 1, f);
	fclose(f);

	for (uint32_t i = 0; i < count; i++) {
		Font_new(font[i], data, pixel_height[i]);
	}

	free(data);
	return ERR_OK;
}

int Font_new(font_t* font, uint8_t* font_buffer, float pixel_height) {
	try_init();

	font->ftex = -1;

	stbtt_fontinfo info;

	int err = stbtt_InitFont(&info, font_buffer, 0);
	if (err == -1) {
		return ERR_CANNOT_LOAD_RESOURCE;
	}

	stbtt_GetFontVMetrics(&info, &font->asc, &font->des, &font->gap);

	uint8_t* temp_bitmap = malloc(FONTTEXTURE_SIZE * FONTTEXTURE_SIZE * sizeof(uint8_t));
	err = stbtt_BakeFontBitmap(font_buffer, 0, pixel_height, temp_bitmap, FONTTEXTURE_SIZE, FONTTEXTURE_SIZE, 32, 96, font->characters);
	if (err == -1) {
		free(temp_bitmap);
		return ERR_CANNOT_LOAD_RESOURCE;
	}
	glGenTextures(1, &font->ftex);
	glBindTexture(GL_TEXTURE_2D, font->ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONTTEXTURE_SIZE, FONTTEXTURE_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	free(temp_bitmap);

	//MEM(font->allocated);
	font->height = pixel_height;
}

float Font_get_line_height(font_t* font) {
	return font->asc - font->des + font->gap;
}

void Font_draw_cstring(font_t* font, const char* str, float x, float y, float scale_x, float scale_y, color_t col) {
	Shader_use(&shader);
	glUniform3f(glGetUniformLocation(shader.program, "textColor"), col[0], col[1], col[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, font->ftex);

	float xdef = x;

	float lineh = Font_get_line_height(font);
	lineh /= font->height;

	while (*str) {
		if (*str == '\n') {
			y += lineh;
			x = xdef;
			++str;
			continue;
		}

		if (*str >= 32 && *str < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(font->characters, FONTTEXTURE_SIZE, FONTTEXTURE_SIZE, *str - 32, &x, &y, &q, 1);
			float vertices[6][4] = {
				{ q.x0, q.y0, q.s0, q.t0 },
				{ q.x0, q.y1, q.s0, q.t1 },
				{ q.x1, q.y1, q.s1, q.t1 },

				{ q.x0, q.y0, q.s0, q.t0 },
				{ q.x1, q.y1, q.s1, q.t1 },
				{ q.x1, q.y0, q.s1, q.t0 },
			};
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		++str;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Font_free(font_t* font) {
	//Mem_release(font->allocated.data);
	glDeleteTextures(1, &font->ftex);
}
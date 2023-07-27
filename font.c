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

void Font_new(font_t* font, const uint8_t* font_buffer, float pixel_height) {
	uint8_t* temp_bitmap = malloc(FONTTEXTURE_SIZE * FONTTEXTURE_SIZE * sizeof(uint8_t));
	int err = stbtt_BakeFontBitmap(font_buffer, 0, pixel_height, temp_bitmap, FONTTEXTURE_SIZE, FONTTEXTURE_SIZE, 32, 96, font->characters);
	glGenTextures(1, &font->ftex);
	glBindTexture(GL_TEXTURE_2D, font->ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONTTEXTURE_SIZE, FONTTEXTURE_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	free(temp_bitmap);

	font->height = pixel_height;

	try_init();
}

void Font_draw_cstring(font_t* font, const char* str, float x, float y, float scale_x, float scale_y, color_t col) {
	Shader_use(&shader);
	glUniform3f(glGetUniformLocation(shader.program, "textColor"), col[0], col[1], col[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, font->ftex);

	while (*str) {
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
	glDeleteTextures(1, &font->ftex);
}
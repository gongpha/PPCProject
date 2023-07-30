#include "maindef.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"

#include "shaders.h"

SHADER_STATIC(shader);
static uint32_t VAO, VBO;

static int initialized = 0;

///////////////////////////

color_t texture_modulate;

void try_init() {
	if (initialized) return;
	initialized = 1;

	// modulate = white
	Texture_reset_modulate();

	const float vertices[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader_create(&shader, TEXTURE_VSHADER, TEXTURE_FSHADER);
}

void Texture_get_shader(shader_t** pptr)
{
	*pptr = &shader;
}

void Texture_new(texture_t* texture) {
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	try_init();
}

const GLint pixelfmt_to_glpixelformat[] = {
	0x00000000, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
};

int Texture_load_from_file(
	texture_t* texture, const char* path
) {
	// load the image from tha PNG file
	int comp;
	uint8_t* data = stbi_load(path, &texture->width, &texture->height, &comp, 0);
	if (!data)
		return ERR_CANNOT_LOAD_RESOURCE;

	int err = Texture_from_data(texture, data, texture->width, texture->height, comp);
	stbi_image_free(data);

	return err;
}

int Texture_from_data(
	texture_t* texture, uint8_t* data, int width, int height, int comp
) {
	// gen opengl texture
	glBindTexture(GL_TEXTURE_2D, texture->id);

	GLint fmt = pixelfmt_to_glpixelformat[comp];
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, texture->width, texture->height, 0, fmt, GL_UNSIGNED_BYTE, data);
	//if (gen_mipmap)
	//	glGenerateMipmap(GL_TEXTURE_2D);

	return ERR_OK;
}

void Texture_delete(texture_t* texture) {
	glDeleteTextures(1, &texture->id);
}

void Texture_set_modulate(color_t color) {
	Color_copy(color, texture_modulate);
}

void Texture_reset_modulate() {
	Color_copy(defcol_white, texture_modulate);
}

void Texture_draw_wsize(texture_t* texture,
	float x, float y,
	float xsize, float ysize
) {
	Shader_use(&shader);
	mat4 model;
	glm_mat4_identity(model);

	glm_translate(model, (vec3) { x, y, 0.0f });
	glm_scale(model, (vec3) { xsize, ysize, 0.0f });

	glUniformMatrix4fv(
		glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, model
	);

	glUniform4fv(
		glGetUniformLocation(shader.program, "modulate"), 1, texture_modulate
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void Texture_draw(texture_t* texture, float x, float y)
{
	Texture_draw_wsize(texture, x, y, texture->width, texture->height);
}

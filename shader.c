#include "maindef.h"

static void check_compiling(GLuint shader, const char* what)
{
	GLint success;
	GLchar infoLog[1024];
	if (strcmp("PROGRAM", what) != 0)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			printf("!!! SHADER COMPILE : %s >> %s\n", what, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			printf("!!! PROGRAM LINK : %s >> %s\n", what, infoLog);
		}
	}
}

void Shader_create(shader_t* shader, const char* vshader, const char* fshader)
{
	uint32_t vertex_shader, fragment_shader;

	// vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vshader, NULL);
	glCompileShader(vertex_shader);
	check_compiling(vertex_shader, "VERTEX");

	// fragment Shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fshader, NULL);
	glCompileShader(fragment_shader);
	check_compiling(fragment_shader, "FRAGMENT");

	shader->program = glCreateProgram();
	glAttachShader(shader->program, vertex_shader);
	glAttachShader(shader->program, fragment_shader);
	glLinkProgram(shader->program);
	check_compiling(shader->program, "PROGRAM");

	// linked. so byeBYE
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader_free(shader_t* shader)
{
	glDeleteProgram(shader->program);
}

void Shader_use(shader_t* shader)
{
	if (shader->program == -1) return;
	glUseProgram(shader->program);
}

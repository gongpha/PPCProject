#include "maindef.h"

#include "glad.h"
#include <GLFW/glfw3.h>

bool_t game_is_server = false; // true if im a server

// for clients
CVAR_CREATE(win_width, "1280");
CVAR_CREATE(win_height, "720");
GLFWwindow* window;
mat4 projection;

void Game_init() {
	cvar_register(&win_width);
	cvar_register(&win_height);
}

void init_modules() {
	Con_init();
}

// just printf . . .
int Game_printf(const char* fmt, ...) {
	va_list arg;
	int done;

	va_start(arg, fmt);
	done = vfprintf(stdout, fmt, arg);
	va_end(arg);

	return done;
}

void Game_init_args(int argc, char** argv) {
	if (argc < 2) { return; }


	for (char** av = argv + 1; *av != argv[argc]; av++) {
		// check each argument
		if (String_MATCH(av, "--server")) {
			// open the dedicated server
			// god
			game_is_server = true;
		}

	}
	Game_printf("WOWAAAAAAAA\n");
}

void loop_window() {
	// CLEAR YO MAMA
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void loop_end() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int Game_loop()
{
	while (!glfwWindowShouldClose(window))
	{
		loop_window();

		Con_draw_console();

		loop_end();
	}
	return 0;
}

int Game_win_get_width()
{
	return win_width.num;
}

int Game_win_get_height()
{
	return win_height.num;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	win_width.num = width;
	win_height.num = height;
	glViewport(0, 0, width, height);

	glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projection);

	shader_t* s;

#define SETPRO(c) c##_get_shader(&s); Shader_use(s); glUniformMatrix4fv(glGetUniformLocation(s->program, "projection"), 1, GL_FALSE, projection)
	SETPRO(Soliddraw);
	SETPRO(Font);
	SETPRO(Texture);
#undef SETPRO
}

int Game_start() {
	// init glfw
	if (!glfwInit())
		return -1;

	// create window
	window = glfwCreateWindow(
		win_width.num,
		win_height.num,
		"PPCProject",
		NULL,
		NULL
	);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//ww = 640;
	//wh = 480;

	// make window current
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	init_modules();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	framebuffer_size_callback(window, win_width.num, win_height.num);

	return 0;
}
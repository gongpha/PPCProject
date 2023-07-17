#include "maindef.h"

#include "glad.h"
#include <GLFW/glfw3.h>

bool_t game_is_server = false; // true if im a server

// for clients
CVAR_CREATE(win_width, "1280");
CVAR_CREATE(win_height, "720");
GLFWwindow* window;

void game__init() {
	cvar_register(&win_width);
	cvar_register(&win_height);
}

// just printf . . .
int game__printf(const char* fmt, ...) {
	va_list arg;
	int done;

	va_start(arg, fmt);
	done = vfprintf(stdout, fmt, arg);
	va_end(arg);

	return done;
}

void game__init_args(int argc, char** argv) {
	if (argc < 2) { return; }


	for (char** av = argv + 1; *av != argv[argc]; av++) {
		// check each argument
		if (String_MATCH(av, "--server")) {
			// open the dedicated server
			// god
			game_is_server = true;
		}

	}
	game__printf("WOWAAAAAAAA\n");
}

void loop_window() {
	// CLEAR YO MAMA
	glClear(GL_COLOR_BUFFER_BIT);
}

void loop_end() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int game__loop()
{
	while (!glfwWindowShouldClose(window))
	{
		loop_window();



		loop_end();
	}
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//window_width = width;
	//window_height = height;
	//toyfest_font_update_viewport(width, height);
	//toyfest_draw_update_viewport(width, height);
	glViewport(0, 0, width, height);
}

int game__start() {
	// init glfw
	if (!glfwInit())
		return -1;

	// create window
	window = glfwCreateWindow(
		cvar_read_num("win_width", 1280),
		cvar_read_num("win_height", 720),
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


	return 0;
}
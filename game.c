#include "maindef.h"

#include "glad.h"
#include <GLFW/glfw3.h>

bool_t game_is_server = false; // true if im a server

bool_t queue_exit = false;

uint32_t frame_processed = 0;

// for clients
CVAR_CREATE(win_width, "1280");
CVAR_CREATE(win_height, "720");

CVAR_CREATE(fps_max, "120");

///////////////////////////////////

GLFWwindow* window;
mat4 projection;

void Game_init() {
	Cvar_register(&win_width);
	Cvar_register(&win_height);
	Cvar_register(&fps_max);
}

void init_modules() {
	Con_init();
	Input_init();
	World_init();

	Con_load_cfg();
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

	World_resize_viewport(width, height);

	if (Con_resize_console(width, height) == ERR_OUT_OF_MEMORY) {
		queue_exit = true;
	}
}

void Game_begin();
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
	Game_begin();

	glfwSetCursorPosCallback(window, Input_process_mouse);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	framebuffer_size_callback(window, win_width.num, win_height.num);

	return 0;
}

double last_sec_frame = 0;
uint32_t last_fps = 0;
uint32_t frame_counter = 0;

double last_timer = 0;
double delta_time = 0;

void Game_frame() {
	// 3D
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	World_draw();
	World_physic_step();

	// 2D
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	Con_draw_console();
}

void Game_begin() {
	Con_make_showing(0);
}

int Game_loop()
{
	while (!glfwWindowShouldClose(window))
	{
		double current_time = glfwGetTime();

		delta_time = current_time - last_timer;
		
		if (current_time - last_timer >= 1.0 / fps_max.num) {
			// CLEAR YO MAMA
			//glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
			glClear(GL_DEPTH_BUFFER_BIT);

			Input_process(window);

			Game_frame();
			glfwSwapBuffers(window);

			frame_processed++;
			last_timer = current_time;
		}

		if (queue_exit)
			break;

		glfwPollEvents();

		if (current_time - last_sec_frame >= 1.0) {
			last_fps = frame_processed - frame_counter;
			frame_counter = frame_processed;
			last_sec_frame = current_time;
			//printf("%d\n", last_fps);
		}
		
	}

	glfwTerminate();
	return 0;
}

float Game_delta()
{
	return delta_time;
}

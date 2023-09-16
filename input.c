#include "maindef.h"

//CONSOLECMD(bind, Input_Bind);

float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;

void Input_init() {
	//CMDREGISTER(bind);
}

void Input_process(GLFWwindow* window)
{
	float speed = 1.0f;// 5200.0f * Game_delta();
	//printf("%f\n", speed);

	Player_forward(_is_pressed(window, GLFW_KEY_W) - _is_pressed(window, GLFW_KEY_S));
	Player_right(_is_pressed(window, GLFW_KEY_D) - _is_pressed(window, GLFW_KEY_A));

	if (_is_pressed(window, GLFW_KEY_SPACE)) {
		Player_jump();
	}
}

int _is_pressed(GLFWwindow* window, int key) {
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void Input_process_mouse(GLFWwindow* window, double xrelad, double yrelad)
{
	float xrela = (float)xrelad;
	float yrela = (float)yrelad;

	float xoffset = xrela - lastX;
	float yoffset = lastY - yrela;
	lastX = xrela;
	lastY = yrela;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.9f)
		pitch = 89.9f;
	if (pitch < -89.9f)
		pitch = -89.9f;

	Camera_update_yawpitch(yaw, pitch);

	
}

//void Input_Bind() {
//
//}
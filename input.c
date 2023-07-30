#include "maindef.h"

//CONSOLECMD(bind, Input_Bind);

float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;

void Input_init() {
	//CMDREGISTER(bind);
}

void Input_process(GLFWwindow* window)
{
	float speed = 520.0f * Game_delta();
	//printf("%f\n", speed);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Camera_forward(speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Camera_forward(-speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Camera_right(-speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Camera_right(speed);
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
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	Camera_update_yawpitch(yaw, pitch);

	
}

//void Input_Bind() {
//
//}
#ifndef _H_INPUT_H_
#define _H_INPUT_H_

#include "glad.h"
#include <GLFW/glfw3.h>

#include "maindef.h"

void Input_process(GLFWwindow* window);
void Input_process_mouse(GLFWwindow* window, double xrela, double yrela);

#endif
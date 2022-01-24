#include "GLFWCallbackFunctions.h"


auto MC_OpenGL::GLFWCallbackFramebufferSize (GLFWwindow *window, int width, int height) -> void
	{
	glViewport (0, 0, width, height);
	}

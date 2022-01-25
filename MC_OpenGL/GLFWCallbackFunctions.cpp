#include "GLFWCallbackFunctions.h"


auto MC_OpenGL::GLFWCallbackFramebufferSize (GLFWwindow *window, int width, int height) -> void
	{
	glViewport (0, 0, width, height);
	}


auto MC_OpenGL::GlfwCallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
		glfwSetWindowShouldClose(window, true);
	if ((key == GLFW_KEY_F2) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		int *drawStyle = reinterpret_cast<int *>(glfwGetWindowUserPointer(window));
		*drawStyle = (*drawStyle == GL_TRIANGLES ? GL_LINE_LOOP : GL_TRIANGLES);
	}
}

#include "GLFWCallbackFunctions.h"
#include "GlobalState.h"


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
        MC_OpenGL::GlobalState *globalState = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));
        switch (globalState->polygonMode)
            {
            case GL_POINT:
                globalState->polygonMode = GL_LINE;
                break;
            case GL_LINE:
            default:
                globalState->polygonMode = GL_FILL;
                break;
            case GL_FILL:
                globalState->polygonMode = GL_POINT;
                break;
            }

        glPolygonMode (GL_FRONT_AND_BACK, globalState->polygonMode);
	}
}

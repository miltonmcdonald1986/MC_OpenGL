#include "GLFWCallbackFunctions.h"
#include "GlobalState.h"

#include <algorithm>
#include <iostream>


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
            case GL_LINE:
            default:
                globalState->polygonMode = GL_FILL;
                break;
            case GL_FILL:
                globalState->polygonMode = GL_LINE;
                break;
            }

        glPolygonMode (GL_FRONT_AND_BACK, globalState->polygonMode);
	}
    if ((key == GLFW_KEY_UP) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        MC_OpenGL::GlobalState* globalState = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));
        globalState->mixPercentage += 0.02;
        globalState->mixPercentage = std::min(globalState->mixPercentage, 1.f);

        globalState->zoom *= 1.1;

        std::cout << globalState->mixPercentage << '\n';
    }
    if ((key == GLFW_KEY_DOWN) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        MC_OpenGL::GlobalState* globalState = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));
        globalState->mixPercentage -= 0.02;
        globalState->mixPercentage = std::max(globalState->mixPercentage, 0.f);

        globalState->zoom *= 0.9;

        std::cout << globalState->mixPercentage << '\n';
    }
}

auto MC_OpenGL::GlfwCallbackCursorEnter (GLFWwindow *window, int entered) -> void
    {
    MC_OpenGL::GlobalState *globalState = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));
    
    double xPos;
    double yPos;
    glfwGetCursorPos (window, &xPos, &yPos);

    globalState->cursorPosXPrev = xPos;
    globalState->cursorPosYPrev = yPos;
    }

auto MC_OpenGL::GlfwCallbackCursorPos (GLFWwindow *window, double xPos, double yPos) -> void
    {
    MC_OpenGL::GlobalState *globalState = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));

    globalState->cursorPosXPrev = globalState->cursorPosX;
    globalState->cursorPosYPrev = globalState->cursorPosY;

    globalState->cursorPosX = xPos;
    globalState->cursorPosY = yPos;
    }


auto MC_OpenGL::GlfwCallbackScroll(GLFWwindow* window, double xoffset, double yoffset) -> void
{
    MC_OpenGL::GlobalState* gs = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));

    gs->zoom *= ( yoffset < 0 ? 1.1 : 0.9 );
}

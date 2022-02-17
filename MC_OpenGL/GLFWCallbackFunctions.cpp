#include "GLFWCallbackFunctions.h"
#include "GlobalState.h"

#include <algorithm>
#include <iostream>


auto MC_OpenGL::GLFWCallbackFramebufferSize (GLFWwindow *window, int width, int height) -> void
	{
	glViewport (0, 0, width, height);

    MC_OpenGL::GlobalState *pGS = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));
    pGS->windowHeight = (float)height;
    pGS->windowWidth = (float)width;
	}

auto MC_OpenGL::GlfwCallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    MC_OpenGL::GlobalState *pGS = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));

	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
		glfwSetWindowShouldClose(window, true);
	if ((key == GLFW_KEY_F2) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
        switch (pGS->polygonMode)
            {
            case GL_LINE:
            default:
                pGS->polygonMode = GL_FILL;
                break;
            case GL_FILL:
                pGS->polygonMode = GL_LINE;
                break;
            }

        glPolygonMode (GL_FRONT_AND_BACK, pGS->polygonMode);
	}
    if ((key == GLFW_KEY_UP) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        pGS->mixPercentage += 0.02;
        pGS->mixPercentage = std::min(pGS->mixPercentage, 1.f);
    }
    if ((key == GLFW_KEY_DOWN) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        pGS->mixPercentage -= 0.02;
        pGS->mixPercentage = std::max(pGS->mixPercentage, 0.f);

        std::cout << pGS->mixPercentage << '\n';
    }
    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS))
        {
        pGS->fit = true;
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
    MC_OpenGL::GlobalState* pGS = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));

    double cx = (pGS->projLeft + pGS->projRight)/2.f;
    double cy = (pGS->projBottom + pGS->projTop)/2.f;

    float dx = pGS->projRight - pGS->projLeft;
    float dy = pGS->projTop - pGS->projBottom;
    if (yoffset > 0)
        {
        dx *= 0.9;
        dy *= 0.9;
        }
    else
        {
        dx *= 1.1;
        dy *= 1.1;
        }
    
    float V = pGS->windowWidth / pGS->windowHeight;
    float A = dx / dy;
    if (V >= A)
        {
        pGS->projLeft = cx - V / A * dx / 2.f;
        pGS->projRight = cx + V / A * dx / 2.f;
        pGS->projBottom = cy - dy / 2.f;
        pGS->projTop = cy + dy / 2.f;
        }
    else
        {
        pGS->projLeft = cx - dx / 2.f;
        pGS->projRight = cx + dx / 2.f;
        pGS->projBottom = cy - A / V * dy / 2.f;
        pGS->projTop = cy + A / V * dy / 2.f;
        }


    //gs->zoom *= ( yoffset < 0 ? 1.1 : 0.9 );

    //std::cout << gs->zoom << '\n';
}

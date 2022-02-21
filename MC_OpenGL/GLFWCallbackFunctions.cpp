#include "GLFWCallbackFunctions.h"

#include <algorithm>
#include <iostream>

#include <glm.hpp>

#include "GlobalState.h"
#include "ProjectionOrthographic.h"


auto MC_OpenGL::GLFWCallbackFramebufferSize (GLFWwindow *window, int width, int height) -> void
	{
	glViewport (0, 0, width, height);

    MC_OpenGL::GlobalState *pGS = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));
    
    float cx = 0.5f*(pGS->projection.GetLeft() + pGS->projection.GetRight());
    float cy = 0.5f*(pGS->projection.GetBottom() + pGS->projection.GetTop());

    // Before updating the window width and height, get the current 
    // projection value and multiply them by newWidth/oldWidth and newHeight/oldHeight
    // so that the objects remain the same size after the window is resized.
    float dx = pGS->projection.GetRight() - pGS->projection.GetLeft();
    float dy = pGS->projection.GetTop() - pGS->projection.GetBottom();

    dx *= (float)width/pGS->windowWidth;
    dy *= (float)height/pGS->windowHeight;

    // Now update the window info in global state.
    pGS->windowHeight = (float)height;
    pGS->windowWidth = (float)width;
    
    UpdateProjection(cx, cy, dx, dy, pGS->projection.GetNear(), pGS->projection.GetFar(), pGS);
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
        pGS->mixPercentage += 0.02f;
        pGS->mixPercentage = std::min(pGS->mixPercentage, 1.f);
    }
    if ((key == GLFW_KEY_DOWN) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        pGS->mixPercentage -= 0.02f;
        pGS->mixPercentage = std::max(pGS->mixPercentage, 0.f);

        std::cout << pGS->mixPercentage << '\n';
    }
    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS))
        {
        pGS->fitAll = true;
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
    MC_OpenGL::GlobalState *pGS = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));

    pGS->cursorPosXPrev = pGS->cursorPosX;
    pGS->cursorPosYPrev = pGS->cursorPosY;

    pGS->cursorPosX = xPos;
    pGS->cursorPosY = yPos;

    float cursorDx = static_cast<float>(pGS->cursorPosX - pGS->cursorPosXPrev);
    float cursorDy = static_cast<float>(pGS->cursorPosY - pGS->cursorPosYPrev);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        float cursorDx = static_cast<float>(pGS->cursorPosX - pGS->cursorPosXPrev);
        float cursorDy = static_cast<float>(pGS->cursorPosY - pGS->cursorPosYPrev);

        float projDx = pGS->projection.GetRight() - pGS->projection.GetLeft();
        float projDy = pGS->projection.GetTop() - pGS->projection.GetBottom();
        
        pGS->projection.SetLeft(pGS->projection.GetLeft() - cursorDx * (projDx) / pGS->windowWidth);
        pGS->projection.SetRight(pGS->projection.GetRight() - cursorDx * (projDx) / pGS->windowWidth);
        pGS->projection.SetBottom(pGS->projection.GetBottom() + cursorDy * (projDy) / pGS->windowHeight);
        pGS->projection.SetTop(pGS->projection.GetTop() + cursorDy * (projDy) / pGS->windowHeight);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
    {
        float angleX = cursorDx * 2.f * glm::pi<float>() / pGS->windowWidth;
        float angleY = cursorDy * 2.f * glm::pi<float>() / pGS->windowHeight;

        pGS->camera.DoArcballRotation(angleX, angleY);
        pGS->fitZOnly = true;
	}
}

auto MC_OpenGL::GlfwCallbackScroll(GLFWwindow* window, double xoffset, double yoffset) -> void
{
    MC_OpenGL::GlobalState* pGS = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));

    double cx = (pGS->projection.GetLeft() + pGS->projection.GetRight())/2.f;
    double cy = (pGS->projection.GetBottom() + pGS->projection.GetTop())/2.f;

    float dx = pGS->projection.GetRight() - pGS->projection.GetLeft();
    float dy = pGS->projection.GetTop() - pGS->projection.GetBottom();

    if (yoffset > 0)
        {
        dx *= 0.9f;
        dy *= 0.9f;
        }
    else if (yoffset < 0)
        {
        dx *= 1.1f;
        dy *= 1.1f;
        }
    
    UpdateProjection(cx, cy, dx, dy, pGS->projection.GetNear(), pGS->projection.GetFar(), pGS);
}

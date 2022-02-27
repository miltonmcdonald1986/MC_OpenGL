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
    pGS->projection.Resize(pGS->windowWidth, pGS->windowHeight, (float)width, (float)height);
    pGS->windowWidth = (float)width;
    pGS->windowHeight = (float)height;
	}

auto MC_OpenGL::GlfwCallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    MC_OpenGL::GlobalState *pGS = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));

    auto FitZAndCenter = [&pGS]()
    {
        pGS->projection.ZoomFit(pGS->drawables, pGS->camera.ViewMatrix(), true);
        pGS->projection.AutoCenter(pGS->drawables, pGS->camera.ViewMatrix());
    };

    if ((mods & GLFW_MOD_ALT) != 0)
    {
        if ((key == GLFW_KEY_B) && (action == GLFW_PRESS))
        {
            pGS->camera.SetViewBack();
            FitZAndCenter();
        }
        if ((key == GLFW_KEY_F) && (action == GLFW_PRESS))
        {
            pGS->camera.SetViewFront();
            FitZAndCenter();
        }
        if ((key == GLFW_KEY_I) && (action == GLFW_PRESS))
        {
            pGS->camera.SetViewIsometric();
            FitZAndCenter();
        }
        if ((key == GLFW_KEY_L) && (action == GLFW_PRESS))
        {
            pGS->camera.SetViewLeft();
            FitZAndCenter();
        }
        if ((key == GLFW_KEY_R) && (action == GLFW_PRESS))
        {
            pGS->camera.SetViewRight();
            FitZAndCenter();
        }
        if ((key == GLFW_KEY_T) && (action == GLFW_PRESS))
        {
            pGS->camera.SetViewTop();
            FitZAndCenter();
        }
    }
    else
    {
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

            glPolygonMode(GL_FRONT_AND_BACK, pGS->polygonMode);
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
            pGS->projection.ZoomFit(pGS->drawables, pGS->camera.ViewMatrix());
        }
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

        int windowWidth;
        int windowHeight;
        glfwGetWindowSize (window, &windowWidth, &windowHeight);

        pGS->projection.Pan (cursorDx, cursorDy);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
    {
        float angleX = cursorDx * 2.f * glm::pi<float>() / pGS->windowWidth;
        float angleY = cursorDy * 2.f * glm::pi<float>() / pGS->windowHeight;

        pGS->camera.DoArcballRotation(angleX, angleY);
        pGS->projection.ZoomFit(pGS->drawables, pGS->camera.ViewMatrix(), true);
	}
}

auto MC_OpenGL::GlfwCallbackScroll(GLFWwindow* window, double xoffset, double yoffset) -> void
{
    MC_OpenGL::GlobalState* pGS = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));
    pGS->projection.ZoomInOutToCursor(yoffset);
}

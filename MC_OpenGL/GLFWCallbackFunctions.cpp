#include "GLFWCallbackFunctions.h"

#include <algorithm>
#include <format>
#include <iostream>

#include <glm.hpp>

#include <Mathematics/ContAlignedBox.h>
#include <Mathematics/IntrLine3AlignedBox3.h>
#include <Mathematics/Line.h>
#include <Mathematics/Vector3.h>

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
        pGS->projection.ZoomFit(pGS, pGS->drawables, pGS->camera.ViewMatrix(), true);
        pGS->projection.AutoCenter(pGS, pGS->drawables, pGS->camera.ViewMatrix());
    };

    if (((mods & GLFW_MOD_ALT) != 0) && ((mods & GLFW_MOD_SHIFT) != 0))
        {
        if ((key == GLFW_KEY_B) && (action == GLFW_PRESS))
            {
            pGS->camera.SetViewBottom ();
            FitZAndCenter ();
            }
        }
    else if ((mods & GLFW_MOD_ALT) != 0)
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
            pGS->projection.ZoomFit(pGS, pGS->drawables, pGS->camera.ViewMatrix());
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

auto ToGteVector3(const glm::vec3& glmVector)
{
    return gte::Vector3<float>({ glmVector.x, glmVector.y, glmVector.z });
}

auto MC_OpenGL::GlfwCallbackCursorPos (GLFWwindow *window, double xPos, double yPos) -> void
    {
    MC_OpenGL::GlobalState *pGS = reinterpret_cast<MC_OpenGL::GlobalState *>(glfwGetWindowUserPointer (window));



    int wx, wy;
    glfwGetWindowSize(window, &wx, &wy);

    ////float zf;
    ////glReadPixels(xPos, yPos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zf);

    glm::vec3 ndc = glm::vec3(xPos / wx, 1.f - yPos / wy, pGS->projection.m_Near) * 2.f - 1.f;
    auto viewSpace = glm::inverse(pGS->projection.ProjectionMatrix()) * glm::vec4(ndc, 1.f);
    auto worldSpaceNear = glm::inverse(pGS->camera.ViewMatrix()) * glm::vec4(viewSpace);

    glfwSetWindowTitle(window, std::format("X{} Y{} Z{}", worldSpaceNear.x, worldSpaceNear.y, worldSpaceNear.z).c_str());

    ndc = glm::vec3(xPos / wx, 1.f - yPos / wy, pGS->projection.m_Far) * 2.f - 1.f;
    viewSpace = glm::inverse(pGS->projection.ProjectionMatrix()) * glm::vec4(ndc, 1.f);
    auto worldSpaceFar = glm::inverse(pGS->camera.ViewMatrix()) * glm::vec4(viewSpace);


    for (int i = 0; i < pGS->drawables.size(); ++i)
    {
        float x0 = std::numeric_limits<float>::max();
        float y0 = std::numeric_limits<float>::max();
        float z0 = std::numeric_limits<float>::max();
        float x1 = std::numeric_limits<float>::lowest();
        float y1 = std::numeric_limits<float>::lowest();
        float z1 = std::numeric_limits<float>::lowest();
        const auto& drawable = pGS->drawables[i];
        const auto& boundingBox = drawable->BoundingBox();
        for (int j = 0; j < boundingBox.size(); ++j)
        {
            glm::vec3 ptWorldSpace = drawable->ModelMatrix() * glm::vec4(boundingBox[j], 1.f);
            x0 = std::min(x0, ptWorldSpace.x);
            y0 = std::min(y0, ptWorldSpace.y);
            x1 = std::max(x1, ptWorldSpace.x);
            y1 = std::max(y1, ptWorldSpace.y);
            z0 = std::min(z0, ptWorldSpace.z);
            z1 = std::max(z1, ptWorldSpace.z);
        }

        gte::Vector3<float> bboxMin({ x0, y0, z0 });
        gte::Vector3<float> bboxMax({ x1, y1, z1 });

        gte::Line3<float> line(ToGteVector3(worldSpaceNear), ToGteVector3(glm::normalize(worldSpaceFar - worldSpaceNear)));
        gte::AlignedBox3<float> box(bboxMin, bboxMax);

        gte::TIQuery<float, gte::Line3<float>, gte::AlignedBox3<float> > tiq;
        if (tiq(line, box).intersect)
            pGS->drawables[i]->SetHover(true);
        else
            pGS->drawables[i]->SetHover(false);
    }

    //auto finalMatrix = glm::inverse(pGS->projection.ProjectionMatrix() * pGS->camera.ViewMatrix());
    //auto wPos = finalMatrix * glm::vec4(ndc, 1.f);
    //auto wPos3 = glm::vec3(wPos)/wPos.w;

    //auto worldSpace = glm::unProject(glm::vec3(xPos, wy - yPos - 1.f, zf), glm::inverse(pGS->camera.ViewMatrix()), pGS->projection.ProjectionMatrix(), glm::vec4(0.f, 0.f, wx, wx));

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
        pGS->projection.ZoomFit(pGS, pGS->drawables, pGS->camera.ViewMatrix(), true);
	}
}

auto MC_OpenGL::GlfwCallbackScroll(GLFWwindow* window, double xoffset, double yoffset) -> void
{
    MC_OpenGL::GlobalState* pGS = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));
    pGS->projection.ZoomInOutToCursor(yoffset);
}

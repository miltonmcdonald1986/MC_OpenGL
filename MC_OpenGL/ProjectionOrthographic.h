#pragma once


#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Drawable.h"


namespace MC_OpenGL
{


	class ProjectionOrthographic
	{
	public:
		ProjectionOrthographic();

		auto AutoCenter(const std::vector<Drawable*>& drawables, const glm::mat4& viewMatrix) -> void;
		auto Pan(float cursorDx, float cursorDy) -> void;
		auto ProjectionMatrix () const -> glm::mat4;
		auto Resize(float oldWidth, float oldHeight, float newWidth, float newHeight) -> void;
		auto SetWindow (GLFWwindow *window) -> void;
		auto ZoomFit(const std::vector<Drawable *> &drawables, const glm::mat4 &viewMatrix, bool fitZOnly = false) -> void;
		auto ZoomInOutToCursor(float offset) -> void;

	private:
		auto UpdateProjectionMatrix(float zNear, float zFar) -> void;
		auto UpdateProjectionMatrix(float aspectRatio, float cx, float cy, float dx, float dy, float zNear, float zFar) -> void;
		auto ZoomInOut(float offset) -> void;

		float m_Left			= -400.f;
		float m_Right			=  400.f;
		float m_Bottom			= -300.f;
		float m_Top				=  300.f;
		float m_Near			=    0.1f;
		float m_Far				=  100.f;
		GLFWwindow *m_Window	= nullptr;
	};


}

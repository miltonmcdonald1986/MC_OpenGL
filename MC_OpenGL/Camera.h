#pragma once


#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


namespace MC_OpenGL
{


	class Camera
	{
	public:
		Camera();

		auto DoArcballRotation(float angleX, float angleY)	-> void;
		auto UpdateViewMatrix()								-> void;
		auto ViewMatrix() const								-> glm::mat4;

	private:
		glm::vec3 m_Eye;
		glm::vec3 m_Center;
		glm::vec3 m_Right;
		glm::vec3 m_Up;
		glm::mat4 m_View;
	};


}

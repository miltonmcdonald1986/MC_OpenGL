#include "Camera.h"


MC_OpenGL::Camera::Camera()
	:	m_Eye	(glm::vec3(0.f, 0.f, 1.f)),
		m_Center(glm::vec3(0.f, 0.f, 0.f)),
		m_Up	(glm::vec3(0.f, 1.f, 0.f)),
		m_Right	(glm::vec3(1.f, 0.f, 0.f)),
		m_View	(glm::lookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)))
{
}


auto MC_OpenGL::Camera::DoArcballRotation(float angleX, float angleY) -> void
{
	glm::vec4 position(m_Eye, 1.f);
	glm::vec4 pivot(m_Center, 1.f);

	m_Eye = glm::rotate(glm::mat4(1.f), -1.f * angleX, m_Up) * (position - pivot) + pivot;
	m_Right = glm::normalize(glm::cross(glm::normalize(m_Eye - m_Center), m_Up));

	position = glm::vec4(m_Eye, 1.f);
	pivot = glm::vec4(m_Center, 1.f);

	m_Eye = glm::rotate(glm::mat4(1.f), 1.f * angleY, m_Right) * (position - pivot) + pivot;
	m_Up = glm::normalize(glm::cross(glm::normalize(m_Center - m_Eye), m_Right));

	UpdateViewMatrix();
}


auto MC_OpenGL::Camera::UpdateViewMatrix() -> void
{
	m_View = glm::lookAt(m_Eye, m_Center, m_Up);
}


auto MC_OpenGL::Camera::ViewMatrix() const -> glm::mat4
{
	return m_View;
}

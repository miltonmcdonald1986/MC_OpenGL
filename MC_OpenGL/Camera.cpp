#include "Camera.h"


namespace {


const glm::vec3 axisX (1.f, 0.f, 0.f);
const glm::vec3 axisY (0.f, 1.f, 0.f);
const glm::vec3 axisZ (0.f, 0.f, 1.f);

const glm::vec3 pointOrigin (0.f, 0.f, 0.f);


}


MC_OpenGL::Camera::Camera()
	:	m_Eye	(axisZ),
		m_Center(pointOrigin),
		m_Up	(axisY),
		m_Right	(axisX),
		m_View	(glm::lookAt(axisZ, pointOrigin, axisY))
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


auto MC_OpenGL::Camera::SetViewIsometric() -> void
{
	auto rootOneThird	= sqrtf(1.f/3.f);

	static const glm::mat3 mat{ 
		     sqrtf(2.f)/2.f,	     sqrtf(2.f)/2.f,	0.f,
		-1.f/sqrtf(6.f),		 1.f/sqrtf(6.f),		2.f/sqrtf(6.f),
		     sqrtf(1.f/3.f),	-1.f*sqrtf(1.f/3.f),	    sqrtf(1.f/3.f)
		};

	m_Right = mat*axisX;
	m_Up	= mat*axisY;
	m_Eye	= mat*axisZ;

	UpdateViewMatrix ();
}


auto MC_OpenGL::Camera::SetViewBack() -> void
{
	m_Right	= -1.f*axisX;
	m_Up	= axisZ;
	m_Eye	= axisY;

	UpdateViewMatrix();
}


auto MC_OpenGL::Camera::SetViewBottom () -> void
	{
	m_Right	= -1.f*axisX;
	m_Up	= axisY;
	m_Eye	= -1.f*axisZ;

	UpdateViewMatrix ();
	}


auto MC_OpenGL::Camera::SetViewFront() -> void
{
	m_Right	= axisX;
	m_Up	= axisZ;
	m_Eye	= -1.f*axisY;

	UpdateViewMatrix();
}


auto MC_OpenGL::Camera::SetViewLeft() -> void
{
	m_Right	= -1.f*axisY;
	m_Up	= axisZ;
	m_Eye	= -1.f*axisX;

	UpdateViewMatrix();
}


auto MC_OpenGL::Camera::SetViewRight() -> void
{
	m_Right	= axisY;
	m_Up	= axisZ;
	m_Eye	= axisX;

	UpdateViewMatrix();
}


auto MC_OpenGL::Camera::SetViewTop() -> void
{
	m_Right	= axisX;
	m_Up	= axisY;
	m_Eye	= axisZ;

	UpdateViewMatrix();
}


auto MC_OpenGL::Camera::ViewMatrix() const -> glm::mat4
{
	return m_View;
}

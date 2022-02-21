#include "ProjectionOrthographic.h"


MC_OpenGL::ProjectionOrthographic::ProjectionOrthographic()
{
}


auto MC_OpenGL::ProjectionOrthographic::ProjectionMatrix() const -> glm::mat4
{
	return glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
}

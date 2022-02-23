#include "ProjectionOrthographic.h"

#include "Drawable.h"


MC_OpenGL::ProjectionOrthographic::ProjectionOrthographic()
{
}


auto MC_OpenGL::ProjectionOrthographic::Pan(float cursorDx, float cursorDy) -> void
{
	float projDx = m_Right - m_Left;
	float projDy = m_Top - m_Bottom;

	int windowWidth;
	int windowHeight;
	glfwGetWindowSize (m_Window, &windowWidth, &windowHeight);

	m_Left		= m_Left	- cursorDx * (projDx) / static_cast<float>(windowWidth);
	m_Right		= m_Right	- cursorDx * (projDx) / static_cast<float>(windowWidth);
	m_Bottom	= m_Bottom	+ cursorDy * (projDy) / static_cast<float>(windowHeight);
	m_Top		= m_Top		+ cursorDy * (projDy) / static_cast<float>(windowHeight);
}


auto MC_OpenGL::ProjectionOrthographic::ProjectionMatrix() const -> glm::mat4
{
	return glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
}


auto MC_OpenGL::ProjectionOrthographic::Resize(float oldWidth, float oldHeight, float newWidth, float newHeight) -> void
{
	float cx = 0.5f * (m_Left + m_Right);
	float cy = 0.5f * (m_Bottom + m_Top);

	// Before updating the window width and height, get the current 
	// projection value and multiply them by newWidth/oldWidth and newHeight/oldHeight
	// so that the objects remain the same size after the window is resized.
	float dx = m_Right - m_Left;
	float dy = m_Top - m_Bottom;

	dx *= newWidth / oldWidth;
	dy *= newHeight / oldHeight;

	UpdateProjectionMatrix(newWidth/newHeight, cx, cy, dx, dy, m_Near, m_Far);
}


auto MC_OpenGL::ProjectionOrthographic::SetWindow (GLFWwindow *window) -> void
{
	m_Window = window;
}


auto MC_OpenGL::ProjectionOrthographic::UpdateProjectionMatrix(float zNear, float zFar) -> void
{
	m_Near = zNear;
	m_Far = zFar;
}


auto MC_OpenGL::ProjectionOrthographic::UpdateProjectionMatrix(float aspectRatio, float cx, float cy, float dx, float dy, float zNear, float zFar) -> void
{
	float V = aspectRatio;
	float A = dx / dy;
	if (V >= A)
	{
		m_Left = cx - 1.f * (V / A) * dx / 2.f;
		m_Right = cx + (V / A) * dx / 2.f;
		m_Bottom = cy - 1.f * dy / 2.f;
		m_Top = cy + dy / 2.f;
		m_Near = zNear;
		m_Far = zFar;
	}
	else
	{
		m_Left = cx - 1.f * dx / 2.f;
		m_Right = cx + dx / 2.f;
		m_Bottom = cy - 1.f * (A / V) * dy / 2.f;
		m_Top = cy + (A / V) * dy / 2.f;
		m_Near = zNear;
		m_Far = zFar;
	}
}


auto MC_OpenGL::ProjectionOrthographic::ZoomFit(GLFWwindow *window, const glm::mat4 &viewMatrix, bool fitZOnly) -> void
{
	float x0 = std::numeric_limits<float>::max();
	float y0 = std::numeric_limits<float>::max();
	float z0 = std::numeric_limits<float>::max();
	float x1 = std::numeric_limits<float>::min();
	float y1 = std::numeric_limits<float>::min();
	float z1 = std::numeric_limits<float>::min();
	for (int i = 0; i < cubePositions.size(); ++i)
	{
		for (int j = 0; j < boundingBox.size(); ++j)
		{
			glm::vec3 ptEyeSpace = viewMatrix * glm::translate(glm::mat4(1.f), cubePositions[i]) * glm::vec4(boundingBox[j], 1.f);
			if (!fitZOnly)
			{
				x0 = std::min(x0, ptEyeSpace.x);
				y0 = std::min(y0, ptEyeSpace.y);
				x1 = std::max(x1, ptEyeSpace.x);
				y1 = std::max(y1, ptEyeSpace.y);
			}
			z0 = std::min(z0, ptEyeSpace.z);
			z1 = std::max(z1, ptEyeSpace.z);
		}
	}

	float cx = 0.5f * (x0 + x1);
	float cy = 0.5f * (y0 + y1);
	float cz = 0.5f * (z0 + z1);

	float dx = x1 - x0;
	dx *= 1.1f;
	float dy = y1 - y0;
	dy *= 1.1f;

	// The z-axis in ortho projection is reversed from the values we just calculated (right-hand vs left-hand thing)
	float zNear = -1.f * z1;
	float zFar = -1.f * z0;

	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	if (fitZOnly)
		UpdateProjectionMatrix(zNear, zFar);
	else
		UpdateProjectionMatrix((float)windowWidth/(float)windowHeight, cx, cy, dx, dy, zNear, zFar);
}


auto MC_OpenGL::ProjectionOrthographic::ZoomInOutToCursor(GLFWwindow* window, float offset) -> void
{
	double xpos;
	double ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int xwid;
	int ywid;
	glfwGetWindowSize(window, &xwid, &ywid);

	float xpct = xpos / xwid;
	float ypct = ypos / ywid;

	float xPctProj = m_Left		+ xpct * (m_Right	- m_Left);
	float yPctProj = m_Bottom	+ ypct * (m_Top		- m_Bottom);

	ZoomInOut(offset);

	float newXPctProj = m_Left		+ xpct * (m_Right	- m_Left);
	float newYPctProj = m_Bottom	+ ypct * (m_Top		- m_Bottom);

	float xShift = newXPctProj - xPctProj;
	float yShift = newYPctProj - yPctProj;

	float xShiftPct = xShift / (m_Right	- m_Left);
	float yShiftPct = yShift / (m_Top	- m_Bottom);
	
	xShiftPct *= xwid;
	yShiftPct *= ywid;

	Pan(xShiftPct, yShiftPct);
}


auto MC_OpenGL::ProjectionOrthographic::ZoomInOut(float offset) -> void
{
	double cx = (m_Left + m_Right) / 2.f;
	double cy = (m_Bottom + m_Top) / 2.f;

	float dx = m_Right - m_Left;
	float dy = m_Top - m_Bottom;

	if (offset > 0)
	{
		dx *= 0.9f;
		dy *= 0.9f;
	}
	else if (offset < 0)
	{
		dx *= 1.1f;
		dy *= 1.1f;
	}

	UpdateProjectionMatrix(dx/dy, cx, cy, dx, dy, m_Near, m_Far);
}

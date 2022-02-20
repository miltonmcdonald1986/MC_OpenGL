#include "ProjectionOrthographic.h"


auto MC_OpenGL::UpdateProjection(float zNear, float zFar, MC_OpenGL::GlobalState* pGS) -> void
{
	pGS->projNear = zNear;
	pGS->projFar = zFar;
}


auto MC_OpenGL::UpdateProjection(float cx, float cy, float dx, float dy, float zNear, float zFar, MC_OpenGL::GlobalState* pGS) -> void
{
	float V = pGS->windowWidth / pGS->windowHeight;
	float A = dx / dy;
	if (V >= A)
	{
		pGS->projLeft = cx - 1.f * (V / A) * dx / 2.f;
		pGS->projRight = cx + (V / A) * dx / 2.f;
		pGS->projBottom = cy - 1.f * dy / 2.f;
		pGS->projTop = cy + dy / 2.f;
		pGS->projNear = zNear;
		pGS->projFar = zFar;
	}
	else
	{
		pGS->projLeft = cx - 1.f * dx / 2.f;
		pGS->projRight = cx + dx / 2.f;
		pGS->projBottom = cy - 1.f * (A / V) * dy / 2.f;
		pGS->projTop = cy + (A / V) * dy / 2.f;
		pGS->projNear = zNear;
		pGS->projFar = zFar;
	}
}

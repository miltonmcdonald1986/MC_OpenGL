#include "ProjectionOrthographic.h"


auto MC_OpenGL::UpdateProjection(float cx, float cy, float dx, float dy, float dz, MC_OpenGL::GlobalState* pGS) -> void
{
	float V = pGS->windowWidth / pGS->windowHeight;
	float A = dx / dy;
	if (V >= A)
	{
		pGS->projLeft = cx - 1.f * (V / A) * dx / 2.f;
		pGS->projRight = cx + (V / A) * dx / 2.f;
		pGS->projBottom = cy - 1.f * dy / 2.f;
		pGS->projTop = cy + dy / 2.f;
		pGS->projNear = -1.f * dz / 2.f;
		pGS->projFar = 1.f * dz / 2.f;
	}
	else
	{
		pGS->projLeft = cx - 1.f * dx / 2.f;
		pGS->projRight = cx + dx / 2.f;
		pGS->projBottom = cy - 1.f * (A / V) * dy / 2.f;
		pGS->projTop = cy + (A / V) * dy / 2.f;
		pGS->projNear = -1.f * dz / 2.f;
		pGS->projFar = 1.f * dz / 2.f;
	}
}

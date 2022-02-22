#include "GlobalState.h"


//auto MC_OpenGL::UpdateProjection(float zNear, float zFar, GlobalState* pGS) -> void
//{
//	pGS->projection.SetNear(zNear);
//	pGS->projection.SetFar(zFar);
//}
//
//
//auto MC_OpenGL::UpdateProjection(float cx, float cy, float dx, float dy, float zNear, float zFar, GlobalState* pGS) -> void
//{
//	float V = pGS->windowWidth / pGS->windowHeight;
//	float A = dx / dy;
//	if (V >= A)
//	{
//		pGS->projection.SetLeft(cx - 1.f * (V / A) * dx / 2.f);
//		pGS->projection.SetRight(cx + (V / A) * dx / 2.f);
//		pGS->projection.SetBottom(cy - 1.f * dy / 2.f);
//		pGS->projection.SetTop(cy + dy / 2.f);
//		pGS->projection.SetNear(zNear);
//		pGS->projection.SetFar(zFar);
//	}
//	else
//	{
//		pGS->projection.SetLeft(cx - 1.f * dx / 2.f);
//		pGS->projection.SetRight(cx + dx / 2.f);
//		pGS->projection.SetBottom(cy - 1.f * (A / V) * dy / 2.f);
//		pGS->projection.SetTop(cy + (A / V) * dy / 2.f);
//		pGS->projection.SetNear(zNear);
//		pGS->projection.SetFar(zFar);
//	}
//}

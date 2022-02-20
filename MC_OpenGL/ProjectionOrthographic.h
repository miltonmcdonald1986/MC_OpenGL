#pragma once


#include "GlobalState.h"


namespace MC_OpenGL
{


	auto UpdateProjection(float zNear, float zFar, MC_OpenGL::GlobalState* pGS) -> void;
	auto UpdateProjection(float cx, float cy, float dx, float dy, float zNear, float zFar, MC_OpenGL::GlobalState* pGS) -> void;


}

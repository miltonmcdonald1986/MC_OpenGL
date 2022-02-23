#pragma once


#include <glad/glad.h>

#include "Camera.h"
#include "ProjectionOrthographic.h"


namespace MC_OpenGL {


struct GlobalState
	{
	Camera					camera			= Camera();
	ProjectionOrthographic	projection		= ProjectionOrthographic();
	double					cursorPosX		= 0.;
	double					cursorPosY		= 0.;
	double					cursorPosXPrev	= 0.;
	double					cursorPosYPrev	= 0.;
	int						polygonMode		= GL_FILL;
	float					mixPercentage	= 0.f;
	float					windowHeight	= 600.f;
	float					windowWidth		= 800.f;
	float					zoom			= 10.f/800.f;
	};


}

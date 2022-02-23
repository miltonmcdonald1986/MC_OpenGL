#pragma once


#include <vector>

#include <glad/glad.h>

#include "Camera.h"
#include "Drawable.h"
#include "ProjectionOrthographic.h"


namespace MC_OpenGL {


struct GlobalState
	{
	Camera								camera			= Camera();
	ProjectionOrthographic				projection		= ProjectionOrthographic();
	std::vector<MC_OpenGL::Drawable *>	drawables		= std::vector<MC_OpenGL::Drawable*>();
	double								cursorPosX		= 0.;
	double								cursorPosY		= 0.;
	double								cursorPosXPrev	= 0.;
	double								cursorPosYPrev	= 0.;
	int									polygonMode		= GL_FILL;
	float								mixPercentage	= 0.f;
	float								windowHeight	= 600.f;
	float								windowWidth		= 800.f;
	float								zoom			= 10.f/800.f;
	};


}

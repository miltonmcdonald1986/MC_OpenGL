#pragma once


namespace MC_OpenGL {


struct GlobalState
	{
	double	cursorPosX		= 0.;
	double	cursorPosY		= 0.;
	double	cursorPosXPrev	= 0.;
	double	cursorPosYPrev	= 0.;
	bool	fit				= false;
	int		polygonMode		= GL_FILL;
	float	projLeft		= -400.f;
	float	projRight		= 400.f;
	float	projBottom		= -300.f;
	float	projTop			= 300.f;
	float	projNear		= 0.1f;
	float	projFar			= 100.f;
	float	mixPercentage	= 0.f;
	float	windowHeight	= 600.f;
	float	windowWidth		= 800.f;
	float	zoom			= 10.f/800.f;
	};


}

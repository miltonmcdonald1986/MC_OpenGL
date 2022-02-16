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
	float	mixPercentage	= 0.f;
	float	windowHeight	= 600.f;
	float	windowWidth		= 800.f;
	float	zoom			= 10.f/800.f;
	};


}

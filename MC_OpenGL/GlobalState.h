#pragma once


namespace MC_OpenGL {


struct GlobalState
	{
	double	cursorPosX		= 0.;
	double	cursorPosY		= 0.;
	double	cursorPosXPrev	= 0.;
	double	cursorPosYPrev	= 0.;
	int		polygonMode		= GL_FILL;
	float	mixPercentage	= 0.f;
	float	zoom			= 0.01f;
	};


}

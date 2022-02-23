#pragma once


#include <iostream>

#include <glad/glad.h>


namespace MC_OpenGL {


class DemoTriangle
	{
	public:
		DemoTriangle ();

		auto Draw () const -> void;

	private:
		GLuint m_ShaderProgram = 0;
		GLuint m_VAO = 0;
	};


}

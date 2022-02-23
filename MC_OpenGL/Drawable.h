#pragma once


#include <array>
#include <vector>

#include <glad/glad.h>

#include <glm.hpp>


namespace MC_OpenGL
{


	auto InitDrawables() -> void;

	extern float vertices[];
	extern GLuint vao;
	extern std::array<glm::vec3, 8> boundingBox;
	extern std::vector<glm::vec3> cubePositions;
	extern unsigned int texture0;
	extern unsigned int texture1;


}

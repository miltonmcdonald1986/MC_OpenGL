#pragma once


#include <array>
#include <vector>

#include <glad/glad.h>

#include <glm.hpp>

#include "Shader.h"


namespace MC_OpenGL
{


	class Drawable
		{
		public:
			virtual auto Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void = 0;
			virtual auto BoundingBox () const -> std::array<glm::vec3, 8> = 0;
			virtual auto ModelMatrix() const -> glm::mat4 = 0;
		};


	class WoodenBox : public Drawable
		{
		public:
			WoodenBox (const glm::mat4 &modelMatrix);

			auto Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void;
			auto BoundingBox () const -> std::array<glm::vec3, 8>;
			auto ModelMatrix() const -> glm::mat4;

		private:
			glm::mat4 m_ModelMatrix = glm::mat4(1.f);
			GLuint m_Vao = 0;
			Shader m_Shader = Shader();
			std::array<glm::vec3, 8> m_BoundingBox = std::array<glm::vec3, 8> ();
		};


	auto InitDrawables() -> void;

	extern float vertices[];
	extern GLuint vao;
	extern std::array<glm::vec3, 8> boundingBox;
	extern std::vector<glm::vec3> cubePositions;
	extern unsigned int texture0;
	extern unsigned int texture1;


}

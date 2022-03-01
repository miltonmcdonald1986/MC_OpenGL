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
			Drawable(GLuint shaderId);

			virtual auto Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void = 0;
			virtual auto BoundingBox () const -> std::array<glm::vec3, 8> = 0;
			virtual auto ModelMatrix() const -> glm::mat4 = 0;

		protected:
			GLuint m_ShaderId = 0;
		};


	class Cube : public Drawable
	{
	public:
		Cube(GLuint shaderId, const glm::mat4& modelMatrix);

		virtual auto Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const -> void;

		auto BoundingBox() const->std::array<glm::vec3, 8>;
		auto ModelMatrix() const->glm::mat4;

	protected:
		glm::mat4					m_ModelMatrix	= glm::mat4(1.f);
		GLuint						m_Vao			= 0;
		std::array<glm::vec3, 8>	m_BoundingBox	= std::array<glm::vec3, 8>();
	};


	class WoodenBox : public Cube
		{
		public:
			WoodenBox (const glm::mat4 &modelMatrix);

			auto Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void;
		};


	auto InitDrawables() -> void;

	extern float vertices[];
	extern GLuint vao;
	extern std::array<glm::vec3, 8> boundingBox;
	extern std::vector<glm::vec3> cubePositions;
	extern unsigned int texture0;
	extern unsigned int texture1;


}

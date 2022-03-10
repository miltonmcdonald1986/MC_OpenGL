#pragma once


#include <array>
#include <vector>

#include <glad/glad.h>

#include <glm.hpp>

#include <Mathematics/Triangle.h>

#include "Shader.h"


namespace MC_OpenGL
{


	enum class DrawableType
	{
		Cube,
		Triangles,
		WoodenBox
	};


	class Drawable
		{
		public:
			Drawable(GLuint shaderId, DrawableType drawableType);

			virtual auto Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void = 0;
			virtual auto BoundingBox () const -> std::array<glm::vec3, 8> = 0;
			virtual auto ModelMatrix() const -> glm::mat4 = 0;

			auto GetColor() const -> glm::vec3;
			auto GetHover() const -> bool;
			auto GetSelected() const -> bool;
			auto GetType() const -> DrawableType;
			auto SetColor(const glm::vec3& rgb) -> void;
			auto SetHover(bool hover) -> void;
			auto SetModel(const glm::mat4 &model) -> void;
			auto SetSelected(bool selected) -> void;

		protected:
			DrawableType m_Type = DrawableType::Cube;
			glm::vec3 m_Color = glm::vec3(0.f, 0.f, 1.f);
			bool m_Hover = false;
			glm::mat4 m_ModelMatrix = glm::mat4(1.f);
			bool m_Selected = false;
			GLuint m_ShaderId = 0;
		};


	class Cube : public Drawable
	{
	public:
		Cube(GLuint shaderId, const glm::mat4& modelMatrix, DrawableType type = DrawableType::Cube);

		virtual auto Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const -> void;

		auto BoundingBox() const->std::array<glm::vec3, 8>;
		auto ModelMatrix() const->glm::mat4;

	protected:
		GLuint						m_Vao			= 0;
		std::array<glm::vec3, 8>	m_BoundingBox	= std::array<glm::vec3, 8>();
	};


	class Triangles : public Drawable
	{
	public:
		Triangles(const Shader& shader, const std::string& stl);

		auto BoundingBox() const -> std::array<glm::vec3, 8>;
		auto Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const -> void;
		auto GetTriangles() const ->std::vector<gte::Triangle3<float> >;
		auto ModelMatrix() const -> glm::mat4;

	private:
		glm::mat4							m_ModelMatrix;
		Shader								m_Shader;
		std::array<glm::vec3, 8>			m_BoundingBox;
		std::vector<int>					m_Indices;
		std::vector<float>					m_Vertices;
		std::vector<gte::Triangle3<float> >	m_Triangles;
		GLuint m_NumVertices;
		GLuint m_Vao = 0;
		GLuint m_Vbo = 0;
		GLuint m_Ebo = 0;
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

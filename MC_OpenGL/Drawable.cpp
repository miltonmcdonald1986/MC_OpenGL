#include "Drawable.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Mathematics/Triangle.h"
#include "Mathematics/Vector3.h"


float MC_OpenGL::vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


std::array<glm::vec3, 8> MC_OpenGL::boundingBox = {
	glm::vec3(-0.5f, -0.5f, -0.5f),
	glm::vec3(-0.5f, -0.5f,  0.5f),
	glm::vec3(-0.5f,  0.5f, -0.5f),
	glm::vec3(-0.5f,  0.5f,  0.5f),
	glm::vec3(0.5f, -0.5f, -0.5f),
	glm::vec3(0.5f, -0.5f,  0.5f),
	glm::vec3(0.5f,  0.5f, -0.5f),
	glm::vec3(0.5f,  0.5f,  0.5f)
};


std::vector<glm::vec3> MC_OpenGL::cubePositions = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};


GLuint MC_OpenGL::vao;
unsigned int MC_OpenGL::texture0;
unsigned int MC_OpenGL::texture1;


auto MC_OpenGL::InitDrawables() -> void
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 180 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// START TEXTURE STUFF

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("..\\textures\\container.jpg", &width, &height, &nrChannels, 0);

	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Error: failed to load texture\n";
		return;// static_cast<int>(MC_OpenGL::ErrorCode::ERROR_TEXTURE_FAILED_TO_LOAD);
	}

	stbi_image_free(data);

	data = stbi_load("..\\textures\\juju.png", &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Error: failed to load texture\n";
		return;// static_cast<int>(MC_OpenGL::ErrorCode::ERROR_TEXTURE_FAILED_TO_LOAD);
	}

	stbi_image_free(data);
}


MC_OpenGL::WoodenBox::WoodenBox (const glm::mat4 &modelMatrix)
	: Cube(Shader(R"(..\shaders\vsContainer.glsl)", R"(..\shaders\fsContainer.glsl)"), modelMatrix, DrawableType::WoodenBox)
	{
	// START TEXTURE STUFF
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load (true);
	unsigned char *data = stbi_load ("..\\textures\\container.jpg", &width, &height, &nrChannels, 0);

	glGenTextures (1, &texture0);
	glBindTexture (GL_TEXTURE_2D, texture0);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
		{
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap (GL_TEXTURE_2D);
		}
	else
		{
		std::cerr << "Error: failed to load texture\n";
		return;// static_cast<int>(MC_OpenGL::ErrorCode::ERROR_TEXTURE_FAILED_TO_LOAD);
		}

	stbi_image_free (data);

	data = stbi_load ("..\\textures\\juju.png", &width, &height, &nrChannels, 0);
	glGenTextures (1, &texture1);
	glBindTexture (GL_TEXTURE_2D, texture1);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
		{
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap (GL_TEXTURE_2D);
		}
	else
		{
		std::cerr << "Error: failed to load texture\n";
		return;// static_cast<int>(MC_OpenGL::ErrorCode::ERROR_TEXTURE_FAILED_TO_LOAD);
		}

	stbi_image_free (data);
	}

	auto MC_OpenGL::WoodenBox::Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void
		{
		glUseProgram(m_ShaderId);

		glBindVertexArray (m_Vao);

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, MC_OpenGL::texture0);

		glActiveTexture (GL_TEXTURE1);
		glBindTexture (GL_TEXTURE_2D, MC_OpenGL::texture1);

		glUniform1i (glGetUniformLocation (m_ShaderId, "samplerContainer"), 0);
		glUniform1i (glGetUniformLocation (m_ShaderId, "samplerAwesomeFace"), 1);

		glUniform1f (glGetUniformLocation (m_ShaderId, "mixPercentage"), 0.f);

		glUniformMatrix4fv (glGetUniformLocation (m_ShaderId, "model"), 1, GL_FALSE, glm::value_ptr (m_ModelMatrix));
		glUniformMatrix4fv (glGetUniformLocation (m_ShaderId, "view"), 1, GL_FALSE, glm::value_ptr (viewMatrix));
		glUniformMatrix4fv (glGetUniformLocation (m_ShaderId, "projection"), 1, GL_FALSE, glm::value_ptr (projectionMatrix));

		glDrawArrays (GL_TRIANGLES, 0, 36);
		}


	MC_OpenGL::Cube::Cube(GLuint shaderId, const glm::mat4& modelMatrix, DrawableType type)
		: Drawable(shaderId, type)
	{
		m_ModelMatrix = modelMatrix;

		float vertices[288] = { // 3 vertex, 2 texture coords, 3 normal
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
		};

		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 288 * sizeof(float), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		m_BoundingBox = std::array<glm::vec3, 8>
		{
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, 0.5f),
			glm::vec3(-0.5f, 0.5f, -0.5f),
			glm::vec3(-0.5f, 0.5f, 0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, 0.5f),
			glm::vec3(0.5f, 0.5f, -0.5f),
			glm::vec3(0.5f, 0.5f, 0.5f)
		};
	}


	auto MC_OpenGL::Cube::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const -> void
	{
		glUseProgram(m_ShaderId);

		glBindVertexArray(m_Vao);

		glUniformMatrix4fv(glGetUniformLocation(m_ShaderId, "model"), 1, GL_FALSE, glm::value_ptr(m_ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderId, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderId, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	auto MC_OpenGL::Cube::BoundingBox () const -> std::array<glm::vec3, 8>
		{
		return m_BoundingBox;
		}


	auto MC_OpenGL::Cube::ModelMatrix () const -> glm::mat4
		{
		return m_ModelMatrix;
		}


	MC_OpenGL::Drawable::Drawable(GLuint shaderId, DrawableType type)
		:	m_ShaderId(shaderId),
			m_Type(type)
	{
	}


	auto MC_OpenGL::Drawable::GetColor() const -> glm::vec3
	{
		return m_Color;
	}


	auto MC_OpenGL::Drawable::GetHover() const -> bool
	{
		return m_Hover;
	}


	auto MC_OpenGL::Drawable::GetSelected() const -> bool
	{
		return m_Selected;
	}

	auto MC_OpenGL::Drawable::GetType() const -> DrawableType
	{
		return m_Type;
	}


	auto MC_OpenGL::Drawable::SetColor(const glm::vec3& rgb) -> void
	{
		m_Color = rgb;
	}


	auto MC_OpenGL::Drawable::SetHover(bool hover) -> void
	{
		m_Hover = hover;
	}


	auto MC_OpenGL::Drawable::SetModel(const glm::mat4& model) -> void
	{
		m_ModelMatrix = model;
	}


	auto MC_OpenGL::Drawable::SetSelected(bool selected) -> void
	{
		m_Selected = selected;
	}


	MC_OpenGL::Triangles::Triangles(const Shader& shader, const std::string& stl)
		: Drawable(shader, DrawableType::Triangles)
	{
		m_Shader = shader;//Shader(R"(..\shaders\vsBasicCoordinateSystems.glsl)", R"(..\shaders\fsBasicCoordinateSystems.glsl)");
		m_ModelMatrix = glm::mat4(1.f);

		std::vector<float> vertices;

		std::ifstream ifs(stl);
		std::string line;
		while (std::getline(ifs, line))
		{
			std::stringstream ss(line);
			std::string firstWord;
			ss >> firstWord;
			std::array<float, 3> normal;
			if (firstWord == "facet")
			{
				ss >> firstWord >> normal[0] >> normal[1] >> normal[2];
			}
			if (firstWord == "vertex")
			{
				float x;
				float y;
				float z;
				ss >> x >> y >> z;

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
				vertices.push_back(0.f);
				vertices.push_back(0.f);
				vertices.push_back(normal[0]);
				vertices.push_back(normal[1]);
				vertices.push_back(normal[2]);
			}
		}
		m_NumVertices = vertices.size() / 8.f;

		int count = 0;
		for (int i = 0; i < vertices.size(); i += 24)
		{
			count++;
			gte::Triangle3<float> tri(
				gte::Vector3<float>({ vertices[i], vertices[i + 1], vertices[i + 2] }),
				gte::Vector3<float>({ vertices[i + 8], vertices[i + 9], vertices[i + 10] }),
				gte::Vector3<float>({ vertices[i + 16], vertices[i + 17], vertices[i + 18] })
			);

			m_Triangles.push_back(tri);
		}

		float x0 = std::numeric_limits<float>::max();
		float y0 = std::numeric_limits<float>::max();
		float z0 = std::numeric_limits<float>::max();
		float x1 = std::numeric_limits<float>::lowest();
		float y1 = std::numeric_limits<float>::lowest();
		float z1 = std::numeric_limits<float>::lowest();
		for (int i = 0; i < vertices.size(); i += 8)
		{
			x0 = std::min(x0, vertices[i]);
			y0 = std::min(y0, vertices[i + 1]);
			x1 = std::max(x1, vertices[i]);
			y1 = std::max(y1, vertices[i + 1]);
			z0 = std::min(z0, vertices[i + 2]);
			z1 = std::max(z1, vertices[i + 2]);
		}
		m_BoundingBox = std::array<glm::vec3, 8>{
			glm::vec3(x0, y0, z0),
				glm::vec3(x0, y0, z1),
				glm::vec3(x0, y1, z0),
				glm::vec3(x0, y1, z1),
				glm::vec3(x1, y0, z0),
				glm::vec3(x1, y0, z1),
				glm::vec3(x1, y1, z0),
				glm::vec3(x1, y1, z1)
		};

		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);

		glGenBuffers(1, &m_Vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}


	auto MC_OpenGL::Triangles::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const -> void
	{
		m_Shader.Use();

		glUniformMatrix4fv(glGetUniformLocation(m_Shader.GetProgramId(), "model"), 1, GL_FALSE, glm::value_ptr(m_ModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_Shader.GetProgramId(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(m_Shader.GetProgramId(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(m_Vao);
		if (m_Indices.empty())
			glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);
		else
			glDrawElements(GL_TRIANGLES, m_NumVertices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	auto MC_OpenGL::Triangles::GetTriangles() const -> std::vector<gte::Triangle3<float>>
	{
		return m_Triangles;
	}


	auto MC_OpenGL::Triangles::ModelMatrix() const -> glm::mat4
	{
		return m_ModelMatrix;
	}

	auto MC_OpenGL::Triangles::BoundingBox() const -> std::array<glm::vec3, 8>
	{
		return m_BoundingBox;
	}

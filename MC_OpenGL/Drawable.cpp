#include "Drawable.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


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
	{
	m_ModelMatrix = modelMatrix;

	float vertices[180] = {
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

	glGenVertexArrays (1, &m_Vao);
	glBindVertexArray (m_Vao);

	GLuint vbo;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 180 * sizeof (float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void *)0);
	glEnableVertexAttribArray (0);

	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void *)(3 * sizeof (float)));
	glEnableVertexAttribArray (1);


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

	m_BoundingBox = std::array<glm::vec3, 8>
		{
		glm::vec3 (-0.5f, -0.5f, -0.5f),
		glm::vec3 (-0.5f, -0.5f,  0.5f),
		glm::vec3 (-0.5f,  0.5f, -0.5f),
		glm::vec3 (-0.5f,  0.5f,  0.5f),
		glm::vec3 (0.5f, -0.5f, -0.5f),
		glm::vec3 (0.5f, -0.5f,  0.5f),
		glm::vec3 (0.5f,  0.5f, -0.5f),
		glm::vec3 (0.5f,  0.5f,  0.5f)
		};

	m_Shader = Shader (R"(..\shaders\vsContainer.glsl)", R"(..\shaders\fsContainer.glsl)");
	if (!m_Shader)
		{
		const auto &[e, i] = m_Shader.GetInfo ();
		std::cerr << i << '\n';
		//return static_cast<int>(e);
		}
	}


	auto MC_OpenGL::WoodenBox::Draw (const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const -> void
		{
		m_Shader.Use ();

		glBindVertexArray (m_Vao);

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, MC_OpenGL::texture0);

		glActiveTexture (GL_TEXTURE1);
		glBindTexture (GL_TEXTURE_2D, MC_OpenGL::texture1);

		glUniform1i (glGetUniformLocation (m_Shader.GetProgramId (), "samplerContainer"), 0);
		glUniform1i (glGetUniformLocation (m_Shader.GetProgramId (), "samplerAwesomeFace"), 1);

		glUniform1f (glGetUniformLocation (m_Shader.GetProgramId (), "mixPercentage"), 0.f);

		glUniformMatrix4fv (glGetUniformLocation (m_Shader.GetProgramId (), "model"), 1, GL_FALSE, glm::value_ptr (m_ModelMatrix));
		glUniformMatrix4fv (glGetUniformLocation (m_Shader.GetProgramId (), "view"), 1, GL_FALSE, glm::value_ptr (viewMatrix));
		glUniformMatrix4fv (glGetUniformLocation (m_Shader.GetProgramId (), "projection"), 1, GL_FALSE, glm::value_ptr (projectionMatrix));

		glDrawArrays (GL_TRIANGLES, 0, 36);
		}


	auto MC_OpenGL::WoodenBox::BoundingBox () const -> std::array<glm::vec3, 8>
		{
		return m_BoundingBox;
		}


	auto MC_OpenGL::WoodenBox::ModelMatrix () const -> glm::mat4
		{
		return m_ModelMatrix;
		}

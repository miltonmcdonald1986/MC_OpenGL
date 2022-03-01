#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <Mathematics/Hyperplane.h>
#include <Mathematics/IntrLine3Plane3.h>
#include <Mathematics/Line.h>
#include <Mathematics/Triangle.h>
#include <Mathematics/Vector3.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#include "Camera.h"
#include "DemoTriangle.h"
#include "Drawable.h"
#include "ErrorCode.h"
#include "GLFWCallbackFunctions.h"
#include "GlobalState.h"
#include "ProjectionOrthographic.h"


std::unique_ptr<MC_OpenGL::GlobalState> pGS;


auto ConvertGlmVec3ToGteVector3(const glm::vec3& glm) -> gte::Vector3<float>
{
	return gte::Vector3<float>({ glm.x, glm.y, glm.z });
}


struct WindowSize
{
	unsigned int width;
	unsigned int height;
};


namespace MC_OpenGL {


	//class TriangleWithColorAttribute : public Drawable
	//	{
	//	public:
	//		TriangleWithColorAttribute (const std::array<gte::Vector3<float>, 3> &vertices, const std::array<gte::Vector3<float>, 3> &colors)
	//			{
	//			glGenVertexArrays (1, &m_Vao);
	//			glBindVertexArray (m_Vao);
	//
	//			glGenBuffers (1, &m_Vbo);
	//			glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
	//			const std::array<float, 18> vboVertices ({
	//				vertices[0][0], vertices[0][1], vertices[0][2], colors[0][0], colors[0][1], colors[0][2],
	//				vertices[1][0], vertices[1][1], vertices[1][2], colors[1][0], colors[1][1], colors[1][2],
	//				vertices[2][0], vertices[2][1], vertices[2][2], colors[2][0], colors[2][1], colors[2][2]
	//				});
	//			glBufferData (GL_ARRAY_BUFFER, vboVertices.size ()*sizeof (float), vboVertices.data (), GL_STATIC_DRAW);
	//
	//			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), nullptr);
	//			glEnableVertexAttribArray (0);
	//
	//			glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *)(3 * sizeof (float)));
	//			glEnableVertexAttribArray (1);
	//			}
	//
	//		auto Draw () const -> void
	//			{
	//			glBindVertexArray (m_Vao);
	//			glDrawArrays (GL_TRIANGLES, 0, 3);
	//			glBindVertexArray (0);
	//			}
	//
	//	private:
	//		GLuint m_Vao = 0;
	//		GLuint m_Vbo = 0;
	//	};
	//
	//
	//class Triangle : public Drawable
	//	{
	//	public:
	//		Triangle (const gte::Vector3<float> &v0, const gte::Vector3<float> &v1, const gte::Vector3<float> &v2)
	//			: Drawable (),
	//			m_GTE_Triangle (v0, v1, v2)
	//			{
	//			glGenVertexArrays (1, &m_Vao);
	//			glBindVertexArray (m_Vao);
	//
	//			glGenBuffers (1, &m_Vbo);
	//			glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
	//			const std::array<float, 9> vertices ({ v0[0], v0[1], v0[2], v1[0], v1[1], v1[2], v2[0], v2[1], v2[2] });
	//			glBufferData (GL_ARRAY_BUFFER, vertices.size () * sizeof (float), vertices.data (), GL_STATIC_DRAW);
	//
	//			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	//			glEnableVertexAttribArray (0);
	//			}
	//
	//		auto Draw () const -> void
	//			{
	//			glBindVertexArray (m_Vao);
	//			glDrawArrays (GL_TRIANGLES, 0, 3);
	//			glBindVertexArray (0);
	//			}
	//
	//	private:
	//		gte::Triangle3<float> m_GTE_Triangle;
	//
	//		GLuint m_Vao = 0;
	//		GLuint m_Vbo = 0;
	//	};


	class Triangles : public Drawable
	{
	public:
		Triangles(const Shader& shader, const std::string& stl)
			: Drawable(shader)
		{
			m_Shader = Shader(R"(..\shaders\vsBasicCoordinateSystems.glsl)", R"(..\shaders\fsBasicCoordinateSystems.glsl)");
			m_ModelMatrix = glm::mat4(1.f);

			std::vector<float> vertices;

			std::ifstream ifs(stl);
			std::string line;
			while (std::getline(ifs, line))
			{
				std::stringstream ss(line);
				std::string firstWord;
				ss >> firstWord;
				if (firstWord == "vertex")
				{
					float x;
					float y;
					float z;
					ss >> x >> y >> z;

					vertices.push_back(x);
					vertices.push_back(y);
					vertices.push_back(z);
				}
			}
			m_NumVertices = vertices.size();

			float x0 = std::numeric_limits<float>::max();
			float y0 = std::numeric_limits<float>::max();
			float z0 = std::numeric_limits<float>::max();
			float x1 = std::numeric_limits<float>::min();
			float y1 = std::numeric_limits<float>::min();
			float z1 = std::numeric_limits<float>::min();
			for (int i = 0; i < vertices.size(); i += 3)
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

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);
		}

		auto Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const -> void
		{
			//GLsizei numVertices = (GLsizei)m_Vertices.size();

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

		auto BoundingBox() const->std::array<glm::vec3, 8>
		{
			return m_BoundingBox;
		}

		auto ModelMatrix() const -> glm::mat4
		{
			return m_ModelMatrix;
		}

	private:
		glm::mat4					m_ModelMatrix;
		Shader						m_Shader;
		std::array<glm::vec3, 8>	m_BoundingBox;
		std::vector<int>			m_Indices;
		std::vector<float>			m_Vertices;
		GLuint m_NumVertices;
		GLuint m_Vao = 0;
		GLuint m_Vbo = 0;
		GLuint m_Ebo = 0;
	};


}

/// <summary> Create a Window to display graphics on. </summary>
///
/// <param name="window"> [in,out] [in,out] A reference to the pointer to the window. Passing as reference
/// 					  because the pointer will be modified. </param>
///
/// <returns> A MC_OpenGL::ErrorCode. </returns>
auto GLFWInit(GLFWwindow*& window, MC_OpenGL::GlobalState* pGS) -> MC_OpenGL::ErrorCode
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const WindowSize windowSize{ 800, 600 };
	window = glfwCreateWindow(windowSize.width, windowSize.height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return MC_OpenGL::ErrorCode::ERROR_GLFW_CREATE_WINDOW;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return MC_OpenGL::ErrorCode::ERROR_GLAD_LOAD_GL_LOADER;
	}

	glViewport(0, 0, windowSize.width, windowSize.height);
	glfwSetFramebufferSizeCallback(window, MC_OpenGL::GLFWCallbackFramebufferSize);
	glfwSetKeyCallback(window, MC_OpenGL::GlfwCallbackKey);
	glfwSetCursorEnterCallback(window, MC_OpenGL::GlfwCallbackCursorEnter);
	glfwSetScrollCallback(window, MC_OpenGL::GlfwCallbackScroll);
	glfwSetCursorPosCallback(window, MC_OpenGL::GlfwCallbackCursorPos);
	glfwSetWindowUserPointer(window, reinterpret_cast<void*>(pGS));

	return MC_OpenGL::ErrorCode::NO_ERROR;
}


int main()
{
	pGS = std::make_unique<MC_OpenGL::GlobalState>();

	GLFWwindow* window = nullptr;
	MC_OpenGL::ErrorCode errorCode = GLFWInit(window, pGS.get());
	pGS->projection.SetWindow(window);

	switch (errorCode)
	{
	case MC_OpenGL::ErrorCode::NO_ERROR:
		if (window == nullptr)
			return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_GLFW_WINDOW_IS_NULL);
		break;
	default:
		return static_cast<int>(errorCode);
	}

	glEnable(GL_DEPTH_TEST);


	MC_OpenGL::InitDrawables();

	MC_OpenGL::Shader shaderAllWhite(R"(..\shaders\vsBasicCoordinateSystems.glsl)", R"(..\shaders\fsAllWhite.glsl)");

	MC_OpenGL::Shader shaderSolidColor(R"(..\shaders\vsBasicCoordinateSystems.glsl)", R"(..\shaders\fsBasicLightColor.glsl)");
	glUseProgram(shaderSolidColor.GetProgramId());
	shaderSolidColor.SetVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));
	shaderSolidColor.SetVec3("objectColor", glm::vec3(0.f, 0.f, 0.5f));

	pGS->drawables.push_back(new MC_OpenGL::Cube(shaderAllWhite.GetProgramId(), glm::translate(glm::mat4(1.f), MC_OpenGL::cubePositions[0])));
	for (int i = 1; i < 10; ++i)
	{
		pGS->drawables.push_back(new MC_OpenGL::Cube(shaderSolidColor.GetProgramId(), glm::translate(glm::mat4(1.f), MC_OpenGL::cubePositions[i])));
	}
	//pGS->drawables.push_back(new MC_OpenGL::Triangles("C:\\Temp\\finalStl.stl"));
	pGS->projection.ZoomFit(pGS->drawables, pGS->camera.ViewMatrix());


	// END TEXTURE STUFF

	glfwGetCursorPos(window, &pGS->cursorPosX, &pGS->cursorPosY);

	MC_OpenGL::DemoTriangle demoTriangle;
	//MC_OpenGL::Triangles triangles("C:\\cncm\\ncfiles\\LT1 090 No Plate.stl");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, pGS->windowWidth, pGS->windowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const MC_OpenGL::Drawable* drawable : pGS->drawables)
			drawable->Draw(pGS->camera.ViewMatrix(), pGS->projection.ProjectionMatrix());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up and exit
	glfwTerminate();

	return 0;
}

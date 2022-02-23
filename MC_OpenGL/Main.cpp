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


auto ConvertGlmVec3ToGteVector3 (const glm::vec3 &glm) -> gte::Vector3<float>
	{
	return gte::Vector3<float> ({ glm.x, glm.y, glm.z });
	}


struct WindowSize
	{
	unsigned int width;
	unsigned int height;
	};


namespace MC_OpenGL {


class TriangleWithColorAttribute : public Drawable
	{
	public:
		TriangleWithColorAttribute (const std::array<gte::Vector3<float>, 3> &vertices, const std::array<gte::Vector3<float>, 3> &colors)
			{
			glGenVertexArrays (1, &m_Vao);
			glBindVertexArray (m_Vao);

			glGenBuffers (1, &m_Vbo);
			glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
			const std::array<float, 18> vboVertices ({
				vertices[0][0], vertices[0][1], vertices[0][2], colors[0][0], colors[0][1], colors[0][2],
				vertices[1][0], vertices[1][1], vertices[1][2], colors[1][0], colors[1][1], colors[1][2],
				vertices[2][0], vertices[2][1], vertices[2][2], colors[2][0], colors[2][1], colors[2][2]
				});
			glBufferData (GL_ARRAY_BUFFER, vboVertices.size ()*sizeof (float), vboVertices.data (), GL_STATIC_DRAW);

			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), nullptr);
			glEnableVertexAttribArray (0);

			glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *)(3 * sizeof (float)));
			glEnableVertexAttribArray (1);
			}

		auto Draw () const -> void
			{
			glBindVertexArray (m_Vao);
			glDrawArrays (GL_TRIANGLES, 0, 3);
			glBindVertexArray (0);
			}

	private:
		GLuint m_Vao = 0;
		GLuint m_Vbo = 0;
	};


class Triangle : public Drawable
	{
	public:
		Triangle (const gte::Vector3<float> &v0, const gte::Vector3<float> &v1, const gte::Vector3<float> &v2)
			: Drawable (),
			m_GTE_Triangle (v0, v1, v2)
			{
			glGenVertexArrays (1, &m_Vao);
			glBindVertexArray (m_Vao);

			glGenBuffers (1, &m_Vbo);
			glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
			const std::array<float, 9> vertices ({ v0[0], v0[1], v0[2], v1[0], v1[1], v1[2], v2[0], v2[1], v2[2] });
			glBufferData (GL_ARRAY_BUFFER, vertices.size () * sizeof (float), vertices.data (), GL_STATIC_DRAW);

			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray (0);
			}

		auto Draw () const -> void
			{
			glBindVertexArray (m_Vao);
			glDrawArrays (GL_TRIANGLES, 0, 3);
			glBindVertexArray (0);
			}

	private:
		gte::Triangle3<float> m_GTE_Triangle;

		GLuint m_Vao = 0;
		GLuint m_Vbo = 0;
	};


class Triangles : public Drawable
	{
	public:

		Triangles (const std::vector<float> &vertices, const std::vector<int> &indices)
			: Drawable (),
			m_Indices (indices),
			m_Vertices (vertices)
			{
			glGenVertexArrays (1, &m_Vao);
			glBindVertexArray (m_Vao);

			glGenBuffers (1, &m_Vbo);
			glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
			glBufferData (GL_ARRAY_BUFFER, vertices.size () * sizeof (float), vertices.data (), GL_STATIC_DRAW);

			glGenBuffers (1, &m_Ebo);
			glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
			glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices.size () * sizeof (float), indices.data (), GL_STATIC_DRAW);

			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray (0);
			}

		Triangles (const std::vector<float> &vertices)
			: Drawable (),
			m_Indices (0),
			m_Vertices (vertices)
			{
			glGenVertexArrays (1, &m_Vao);
			glBindVertexArray (m_Vao);

			glGenBuffers (1, &m_Vbo);
			glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
			glBufferData (GL_ARRAY_BUFFER, vertices.size () * sizeof (float), vertices.data (), GL_STATIC_DRAW);

			glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray (0);
			}

		auto Draw () const -> void
			{
			GLsizei numVertices = (GLsizei)m_Vertices.size();

			glBindVertexArray (m_Vao);
			if (m_Indices.empty ())
				glDrawArrays (GL_TRIANGLES, 0, numVertices);
			else
				glDrawElements (GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
			glBindVertexArray (0);
			}

	private:
		std::vector<int>    m_Indices;
		std::vector<float>  m_Vertices;

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
auto GLFWInit (GLFWwindow *&window, MC_OpenGL::GlobalState *pGS) -> MC_OpenGL::ErrorCode
	{

	glfwInit ();
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const WindowSize windowSize { 800, 600 };
	window = glfwCreateWindow (windowSize.width, windowSize.height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
		{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate ();
		return MC_OpenGL::ErrorCode::ERROR_GLFW_CREATE_WINDOW;
		}
	glfwMakeContextCurrent (window);

	if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress))
		{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return MC_OpenGL::ErrorCode::ERROR_GLAD_LOAD_GL_LOADER;
		}

	glViewport (0, 0, windowSize.width, windowSize.height);
	glfwSetFramebufferSizeCallback (window, MC_OpenGL::GLFWCallbackFramebufferSize);
	glfwSetKeyCallback (window, MC_OpenGL::GlfwCallbackKey);
	glfwSetCursorEnterCallback (window, MC_OpenGL::GlfwCallbackCursorEnter);
	glfwSetScrollCallback(window, MC_OpenGL::GlfwCallbackScroll);
	glfwSetCursorPosCallback (window, MC_OpenGL::GlfwCallbackCursorPos);
	glfwSetWindowUserPointer (window, reinterpret_cast<void *>(pGS));

	return MC_OpenGL::ErrorCode::NO_ERROR;
	}


int main ()
	{
	pGS = std::make_unique<MC_OpenGL::GlobalState> ();

	GLFWwindow *window = nullptr;
	MC_OpenGL::ErrorCode errorCode = GLFWInit (window, pGS.get());
	pGS->projection.SetWindow (window);

	switch (errorCode)
		{
		case MC_OpenGL::ErrorCode::NO_ERROR:
			if (window == nullptr)
				return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_GLFW_WINDOW_IS_NULL);
			break;
		default:
			return static_cast<int>(errorCode);
		}

	glEnable (GL_DEPTH_TEST);


	MC_OpenGL::InitDrawables();
	pGS->projection.ZoomFit(pGS->camera.ViewMatrix());


	// END TEXTURE STUFF
	
	glfwGetCursorPos (window, &pGS->cursorPosX, &pGS->cursorPosY);

	MC_OpenGL::DemoTriangle demoTriangle;

	std::vector<MC_OpenGL::Drawable *> drawables;
	for (int i = 0; i < 10; ++i)
		{
		drawables.push_back (new MC_OpenGL::WoodenBox (glm::translate (glm::mat4 (1.f), MC_OpenGL::cubePositions[i])));
		}

	// Game loop
	while (!glfwWindowShouldClose (window))
		{
		glViewport (0, 0, pGS->windowWidth, pGS->windowHeight);
		glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const MC_OpenGL::Drawable *drawable : drawables)
			drawable->Draw (pGS->camera.ViewMatrix (), pGS->projection.ProjectionMatrix ());

		//// Draw pip window for fun
		//glViewport (40, 30, 160, 120);
		//glScissor (40, 30, 160, 120);
		//glEnable (GL_SCISSOR_TEST);
		//glClear (GL_DEPTH_BUFFER_BIT);
		//glDisable (GL_SCISSOR_TEST);
		////demoTriangle.Draw ();

		//glm::mat4 model = glm::mat4 (1.f);
		//model = glm::rotate (model, (float)glfwGetTime (), glm::vec3( 0.1f, 0.2f, 0.3f ));
		//model = glm::translate (model, MC_OpenGL::cubePositions[0]);

		//glm::mat4 view = pGS->camera.ViewMatrix ();// glm::mat4 (1.f);
		//glm::mat4 projection = glm::ortho(-sqrtf(2.f), sqrtf (2.f), -sqrtf (2.f), sqrtf (2.f), -sqrtf (2.f), sqrtf (2.f));

		//glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "model"), 1, GL_FALSE, glm::value_ptr (model));
		//glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "view"), 1, GL_FALSE, glm::value_ptr (view));
		//glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "projection"), 1, GL_FALSE, glm::value_ptr (projection));

		//glDrawArrays (GL_TRIANGLES, 0, 36);

		glfwSwapBuffers (window);
		glfwPollEvents ();
		}

	// Clean up and exit
	glfwTerminate ();

	return 0;
	}

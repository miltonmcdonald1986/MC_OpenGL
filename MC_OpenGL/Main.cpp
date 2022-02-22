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
#include "Drawable.h"
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


enum class ErrorCode
	{
	NO_ERROR,
	ERROR_FRAGMENT_SHADER_COMPILATION_FAILED,
	ERROR_GLAD_LOAD_GL_LOADER,
	ERROR_GLFW_CREATE_WINDOW,
	ERROR_GLFW_WINDOW_IS_NULL,
	ERROR_SHADER_PROGRAM_LINKING_FAILED,
	ERROR_TEXTURE_FAILED_TO_LOAD,
	ERROR_VERTEX_SHADER_COMPILATION_FAILED
	};


class Drawable
	{
	public:
		Drawable ()
			: m_Vao (0),
			m_Vbo (0)
			{
			}

		virtual auto Draw () const -> void = 0;

	protected:
		GLuint m_Ebo;
		GLuint m_Vao;
		GLuint m_Vbo;
	};


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


class Shader
	{
	public:
		Shader (const std::string &vsFilename, const std::string &fsFilename)
			: m_ErrorCode (MC_OpenGL::ErrorCode::NO_ERROR)
			{
			std::ifstream ifs (vsFilename);
			std::string line;
			std::stringstream ss;
			while (std::getline (ifs, line))
				ss << line << '\n';
			ss << '\0';
			ifs.close ();
			
			GLuint vsId = glCreateShader (GL_VERTEX_SHADER);
			std::string strVsSourceCode = ss.str ();
			const char *vsSourceCode = strVsSourceCode.c_str();
			glShaderSource (vsId, 1, &vsSourceCode, NULL);
			glCompileShader (vsId);
			GLint success;
			const int infoLogSize = 512;
			GLchar infoLog[infoLogSize];
			glGetShaderiv (vsId, GL_COMPILE_STATUS, &success);
			if (!success)
				{
				glGetShaderInfoLog (vsId, infoLogSize, nullptr, infoLog);
				std::stringstream ssInfoLog;
				ssInfoLog << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
				m_InfoLog = ssInfoLog.str ();
				m_ErrorCode = MC_OpenGL::ErrorCode::ERROR_VERTEX_SHADER_COMPILATION_FAILED;
				return;
				}

			ifs.open (fsFilename);
			ss = std::stringstream ();
			while (std::getline (ifs, line))
				ss << line << '\n';
			ss << '\0';
			ifs.close ();
			
			GLuint fsId = glCreateShader (GL_FRAGMENT_SHADER);
			std::string strFsSourceCode = ss.str ();
			const char *fsSourceCode = strFsSourceCode.c_str();
			glShaderSource (fsId, 1, &fsSourceCode, nullptr);
			glCompileShader (fsId);
			glGetShaderiv (fsId, GL_COMPILE_STATUS, &success);
			if (!success)
				{
				glGetShaderInfoLog (fsId, infoLogSize, nullptr, infoLog);
				std::stringstream ssInfoLog;
				ssInfoLog << "Error: Fragment shader compilation failed\n" << infoLog << std::endl;
				m_InfoLog = ssInfoLog.str ();
				m_ErrorCode = MC_OpenGL::ErrorCode::ERROR_FRAGMENT_SHADER_COMPILATION_FAILED;
				return;
				}

			m_Id = glCreateProgram ();
			glAttachShader (m_Id, vsId);
			glAttachShader (m_Id, fsId);
			glLinkProgram (m_Id);
			glGetShaderiv (m_Id, GL_LINK_STATUS, &success);
			if (!success)
				{
				glGetProgramInfoLog (m_Id, infoLogSize, nullptr, infoLog);
				std::stringstream ssInfoLog;
				ssInfoLog << "Error: Shader program linking failed\n" << infoLog << std::endl;
				m_InfoLog = ssInfoLog.str ();
				m_ErrorCode = MC_OpenGL::ErrorCode::ERROR_SHADER_PROGRAM_LINKING_FAILED;
				}

			glDeleteShader (vsId);
			glDeleteShader (vsId);
			}

		operator bool () const
			{
			return m_ErrorCode == MC_OpenGL::ErrorCode::NO_ERROR;
			}

		auto GetProgramId () const
			{
			return m_Id;
			}

		auto Use () const
			{
			glUseProgram (m_Id);
			}

		auto GetInfo () const -> std::pair<MC_OpenGL::ErrorCode, std::string>
			{
			return { m_ErrorCode, m_InfoLog };
			}

		auto SetUniformFloat1f (const std::string name, float value) const -> void
			{
			glUniform1f (glGetUniformLocation (m_Id, name.c_str ()), value);
			}

	private:
		GLuint                  m_Id;
		MC_OpenGL::ErrorCode    m_ErrorCode;
		std::string             m_InfoLog;
	};


int main ()
	{
	pGS = std::make_unique<MC_OpenGL::GlobalState> ();

	GLFWwindow *window = nullptr;
	MC_OpenGL::ErrorCode errorCode = GLFWInit (window, pGS.get());
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

	Shader shader (R"(..\shaders\vsContainer.glsl)", R"(..\shaders\fsContainer.glsl)");
	if (!shader)
		{
		const auto &[e, i] = shader.GetInfo ();
		std::cerr << i << '\n';
		return static_cast<int>(e);
		}


	MC_OpenGL::InitDrawables();
	pGS->projection.ZoomFit(window, pGS->camera.ViewMatrix());


	// END TEXTURE STUFF
	
	glfwGetCursorPos (window, &pGS->cursorPosX, &pGS->cursorPosY);

	//pGS->fitAll = true;

	// Game loop
	while (!glfwWindowShouldClose (window))
		{
		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();

		glBindVertexArray(MC_OpenGL::vao);

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, MC_OpenGL::texture0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MC_OpenGL::texture1);

		glUniform1i (glGetUniformLocation (shader.GetProgramId (), "samplerContainer"), 0);
		glUniform1i (glGetUniformLocation (shader.GetProgramId (), "samplerAwesomeFace"), 1);

		glUniform1f (glGetUniformLocation (shader.GetProgramId (), "mixPercentage"), pGS->mixPercentage);

		for (int i = 0; i < 10; ++i)
			{
			glm::mat4 model = glm::mat4 (1.f);
			model = glm::translate (model, MC_OpenGL::cubePositions[i]);
			
			glm::mat4 view = pGS->camera.ViewMatrix();// glm::mat4 (1.f);

			glm::mat4 projection = pGS->projection.ProjectionMatrix();

			glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "model"), 1, GL_FALSE, glm::value_ptr (model));
			glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "view"), 1, GL_FALSE, glm::value_ptr (view));
			glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "projection"), 1, GL_FALSE, glm::value_ptr (projection));

			glDrawArrays (GL_TRIANGLES, 0, 36);
			}

		glfwSwapBuffers (window);
		glfwPollEvents ();
		}

	// Clean up and exit
	glfwTerminate ();

	return 0;
	}

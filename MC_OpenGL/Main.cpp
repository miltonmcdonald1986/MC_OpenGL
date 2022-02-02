#include <array>
#include <iostream>
#include <sstream>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <Mathematics/Triangle.h>
#include <Mathematics/Vector3.h>

#include "GLFWCallbackFunctions.h"
#include "GlobalState.h"

MC_OpenGL::GlobalState globalState;


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
			glBindVertexArray (m_Vao);
			if (m_Indices.empty ())
				glDrawArrays (GL_TRIANGLES, 0, m_Vertices.size ());
			else
				glDrawElements (GL_TRIANGLES, m_Vertices.size (), GL_UNSIGNED_INT, 0);
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
auto GLFWInit (GLFWwindow *&window) -> MC_OpenGL::ErrorCode
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
	glfwSetWindowUserPointer (window, reinterpret_cast<void *>(&globalState));

	return MC_OpenGL::ErrorCode::NO_ERROR;
	}


class Shader
	{
	public:
		Shader (const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
			: m_ErrorCode (MC_OpenGL::ErrorCode::NO_ERROR)
			{
			GLuint ID_vertexShader = glCreateShader (GL_VERTEX_SHADER);
			const char *vertexShaderCStr = vertexShaderSource.data ();
			glShaderSource (ID_vertexShader, 1, &vertexShaderCStr, NULL);
			glCompileShader (ID_vertexShader);
			GLint success;
			const int infoLogSize = 512;
			GLchar infoLog[infoLogSize];
			glGetShaderiv (ID_vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
				{
				glGetShaderInfoLog (ID_vertexShader, infoLogSize, nullptr, infoLog);
				std::stringstream ssInfoLog;
				ssInfoLog << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
				m_InfoLog = ssInfoLog.str ();
				m_ErrorCode = MC_OpenGL::ErrorCode::ERROR_VERTEX_SHADER_COMPILATION_FAILED;
				return;
				}

			GLuint ID_fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
			const char *fragmentShaderCStr = fragmentShaderSource.data ();
			glShaderSource (ID_fragmentShader, 1, &fragmentShaderCStr, nullptr);
			glCompileShader (ID_fragmentShader);
			glGetShaderiv (ID_fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
				{
				glGetShaderInfoLog (ID_fragmentShader, infoLogSize, nullptr, infoLog);
				std::stringstream ssInfoLog;
				ssInfoLog << "Error: Fragment shader compilation failed\n" << infoLog << std::endl;
				m_InfoLog = ssInfoLog.str ();
				m_ErrorCode = MC_OpenGL::ErrorCode::ERROR_FRAGMENT_SHADER_COMPILATION_FAILED;
				return;
				}

			m_Id = glCreateProgram ();
			glAttachShader (m_Id, ID_vertexShader);
			glAttachShader (m_Id, ID_fragmentShader);
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

			glDeleteShader (ID_vertexShader);
			glDeleteShader (ID_fragmentShader);
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
	GLFWwindow *window = nullptr;
	MC_OpenGL::ErrorCode errorCode = GLFWInit (window);
	switch (errorCode)
		{
		case MC_OpenGL::ErrorCode::NO_ERROR:
			if (window == nullptr)
				return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_GLFW_WINDOW_IS_NULL);
			break;
		default:
			return static_cast<int>(errorCode);
		}

	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"uniform float posY;\n"
		"out vec3 ourColor;\n"
		"void main ()\n"
		"{\n"
		"gl_Position = vec4 (aPos.x + 0.5f, -1.f*(aPos.y + posY), -1.f*aPos.z, 1.f);\n"
		"ourColor = vec3(gl_Position.x, gl_Position.y, 1.0f);\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"void main ()\n"
		"{\n"
		"FragColor = vec4(ourColor, 1.f);\n"
		"}\0";

	Shader shader (vertexShaderSource, fragmentShaderSource);
	if (!shader)
		{
		const auto &[e, i] = shader.GetInfo ();
		std::cerr << i << '\n';
		return static_cast<int>(e);
		}

	MC_OpenGL::TriangleWithColorAttribute coloredTriangle (
		{	gte::Vector3<float> ({  0.5f, -0.5f, 0.f }),
			gte::Vector3<float> ({ -0.5f, -0.5f, 0.f }),
			gte::Vector3<float> ({  0.f,   0.5f, 0.f }) }, 
		{
			gte::Vector3<float> ({ 1.f, 0.f, 0.f }),
			gte::Vector3<float> ({ 0.f, 1.f, 0.f }),
			gte::Vector3<float> ({ 0.f, 0.f, 1.f })
		});

	// Game loop
	double previousTime = glfwGetTime ();
	while (!glfwWindowShouldClose (window))
		{
		float timeValue = glfwGetTime ();
		float posY = (sin (timeValue) / 2.0f);
		
		std::cout << posY << '\n';

		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT);

		shader.Use ();
		shader.SetUniformFloat1f ("posY", posY);
		coloredTriangle.Draw ();

		//glUseProgram (shader1.GetProgramId ());
		//triangle.Draw ();

		glfwSwapBuffers (window);
		glfwPollEvents ();
		}

	// Clean up and exit
	glfwTerminate ();

	return 0;
	}

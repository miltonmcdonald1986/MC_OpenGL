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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GLFWCallbackFunctions.h"
#include "GlobalState.h"
#include "ProjectionOrthographic.h"


std::unique_ptr<MC_OpenGL::GlobalState> pGS;


class Camera
	{
	public:
		Camera ()
			:	m_Eye(glm::vec3(0.f, 0.f, 1.f)),
				m_Center(glm::vec3(0.f, 0.f, 0.f)),
				m_Up(glm::vec3(0.f, 1.f, 0.f)),
				m_Right(glm::vec3(1.f, 0.f, 0.f))
			{
			glm::vec4 position(m_Eye, 1.f);
			glm::vec4 pivot(m_Center, 1.f);

			float angleX = 0.f;
			float angleY = 0.f;

			m_Eye = glm::rotate(glm::mat4(1.f), -1.f * angleX, m_Up) * (position - pivot) + pivot;
			m_Right = glm::normalize(glm::cross(glm::normalize(m_Eye - m_Center), m_Up));

			position = glm::vec4(m_Eye, 1.f);
			pivot = glm::vec4(m_Center, 1.f);

			m_Eye = glm::rotate(glm::mat4(1.f), 1.f * angleY, m_Right) * (position - pivot) + pivot;
			m_Up = glm::normalize(glm::cross(glm::normalize(m_Center - m_Eye), m_Right));

			m_View = glm::lookAt (m_Eye, m_Center, m_Up);
			}

		auto Update (GLFWwindow *window)
			{
			MC_OpenGL::GlobalState* gs = reinterpret_cast<MC_OpenGL::GlobalState*>(glfwGetWindowUserPointer(window));

			float cursorDeltaX = gs->cursorPosX - gs->cursorPosXPrev;
			float cursorDeltaY = gs->cursorPosY - gs->cursorPosYPrev;

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
			{
				glm::vec4 position(m_Eye, 1.f);
				glm::vec4 pivot(m_Center, 1.f);

				float angleX = cursorDeltaX * 2.f * glm::pi<float>() / 800.f;
				float angleY = cursorDeltaY * 2.f * glm::pi<float>() / 600.f;

				m_Eye = glm::rotate(glm::mat4(1.f), -1.f*angleX, m_Up)*(position - pivot) + pivot;
				m_Right = glm::normalize(glm::cross(glm::normalize(m_Eye - m_Center), m_Up));

				position = glm::vec4(m_Eye, 1.f);
				pivot = glm::vec4(m_Center, 1.f);

				m_Eye = glm::rotate(glm::mat4(1.f), 1.f*angleY, m_Right)*(position - pivot) + pivot;
				m_Up = glm::normalize(glm::cross(glm::normalize(m_Center - m_Eye), m_Right));
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
			{
				float pct = cursorDeltaX*gs->zoom;
				//pct = pct;
				//m_Center += pct*m_Right;
				//m_Eye += pct*m_Right;

				pGS->projLeft -= cursorDeltaX*(pGS->projRight - pGS->projLeft)/pGS->windowWidth;
				pGS->projRight -= cursorDeltaX*(pGS->projRight - pGS->projLeft)/pGS->windowWidth;

				//pct = cursorDeltaY*gs->zoom;
				//pct = pct;
				//m_Center += pct * m_Up;
				//m_Eye += pct * m_Up;
				
				pGS->projBottom += cursorDeltaY*(pGS->projTop - pGS->projBottom)/pGS->windowHeight;
				pGS->projTop += cursorDeltaY*(pGS->projTop - pGS->projBottom)/pGS->windowHeight;
			}

			m_View = glm::lookAt (m_Eye, m_Center, m_Up);
			}

		auto GetEye () const -> glm::vec3
			{
			return m_Eye;
			}

		auto GetView () const -> glm::mat4
			{
			return m_View;
			}

		auto GetViewDir () const -> glm::vec3
			{
			return glm::normalize (m_Center - m_Eye);
			}

		auto Translate(const glm::vec3 &toPoint) -> void
		{
			auto diff = toPoint - m_Eye;
			m_Eye += diff;
			m_Center += diff;
		}


	private:
		glm::mat4 m_View;
		glm::vec3 m_Eye;
		glm::vec3 m_Center;
		glm::vec3 m_Up;
		glm::vec3 m_Right;
	};


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
	//glfwSetCursorPosCallback (window, MC_OpenGL::GlfwCallbackCursorPos);
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

	//float vertices[] = {
 //   // positions          // colors           // texture coords
 //    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
 //    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
 //   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
 //   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	//};
	//unsigned int indices[] = {  // note that we start from 0!
 //   0, 1, 3,   // first triangle
 //   1, 2, 3    // second triangle
	//}; 
	
	float vertices[] = {
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

	std::array<glm::vec3, 8> boundingBox = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f)
	};

	glm::vec3 cubePositions[] = {
	glm::vec3 (0.0f,  0.0f,  0.0f),
	glm::vec3 (2.0f,  5.0f, -15.0f),
	glm::vec3 (-1.5f, -2.2f, -2.5f),
	glm::vec3 (-3.8f, -2.0f, -12.3f),
	glm::vec3 (2.4f, -0.4f, -3.5f),
	glm::vec3 (-1.7f,  3.0f, -7.5f),
	glm::vec3 (1.3f, -2.0f, -2.5f),
	glm::vec3 (1.5f,  2.0f, -2.5f),
	glm::vec3 (1.5f,  0.2f, -1.5f),
	glm::vec3 (-1.3f,  1.0f, -1.5f)
		};

	GLuint vao;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);

	GLuint vbo;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 180*sizeof(float), vertices, GL_STATIC_DRAW);

	//GLuint ebo;
	//glGenBuffers (1, &ebo);
	//glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData (GL_ELEMENT_ARRAY_BUFFER, 6*sizeof (int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);

	//glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof (float)));
	//glEnableVertexAttribArray (1);

	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5*sizeof (float), (void *)(3*sizeof (float)));
	glEnableVertexAttribArray (1);

	
	// START TEXTURE STUFF
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load ("..\\textures\\container.jpg", &width, &height, &nrChannels, 0);

	unsigned int texture0;
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
		return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_TEXTURE_FAILED_TO_LOAD);
		}

	stbi_image_free (data);

	data = stbi_load("..\\textures\\juju.png", &width, &height, &nrChannels, 0);
	unsigned int texture1;
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
		return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_TEXTURE_FAILED_TO_LOAD);
	}

	stbi_image_free(data);

	// END TEXTURE STUFF
	
	glfwGetCursorPos (window, &pGS->cursorPosX, &pGS->cursorPosY);


	Camera camera;
	pGS->fit = true;

	// Game loop
	while (!glfwWindowShouldClose (window))
		{
		glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update Mouse BEFORE updating camera.
		pGS->cursorPosXPrev = pGS->cursorPosX;
		pGS->cursorPosYPrev = pGS->cursorPosY;
		glfwGetCursorPos (window, &pGS->cursorPosX, &pGS->cursorPosY);

		camera.Update (window);

		shader.Use();

		glBindVertexArray(vao);

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, texture0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glUniform1i (glGetUniformLocation (shader.GetProgramId (), "samplerContainer"), 0);
		glUniform1i (glGetUniformLocation (shader.GetProgramId (), "samplerAwesomeFace"), 1);

		glUniform1f (glGetUniformLocation (shader.GetProgramId (), "mixPercentage"), pGS->mixPercentage);

		if (pGS->fit)
			{
			float x0 = std::numeric_limits<float>::max ();
			float y0 = std::numeric_limits<float>::max ();
			float z0 = std::numeric_limits<float>::max ();
			float x1 = std::numeric_limits<float>::min ();
			float y1 = std::numeric_limits<float>::min ();
			float z1 = std::numeric_limits<float>::min ();
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < boundingBox.size(); ++j)
				{
					glm::vec3 ptEyeSpace = camera.GetView() * glm::translate(glm::mat4(1.f), cubePositions[i]) * glm::vec4(boundingBox[j], 1.f);
					x0 = std::min(x0, ptEyeSpace.x);
					y0 = std::min(y0, ptEyeSpace.y);
					z0 = std::min(z0, ptEyeSpace.z);
					x1 = std::max(x1, ptEyeSpace.x);
					y1 = std::max(y1, ptEyeSpace.y);
					z1 = std::max(z1, ptEyeSpace.z);
				}
			}

			float cx = 0.5f * (x0 + x1);
			float cy = 0.5f * (y0 + y1);
			float cz = 0.5f * (z0 + z1);

			float dx = x1 - x0;
			dx *= 1.1f;
			float dy = y1 - y0;
			dy *= 1.1f;
			float dz = 2.f*(z1 - z0); // I like to double dz on fit so that the objects won't get clipped if I rotate the camera around.
			dz *= 1.1f;

			MC_OpenGL::UpdateProjection(cx, cy, dx, dy, dz, pGS.get());

			//float V = pGS->windowWidth/pGS->windowHeight;
			//float A = dx/dy;
			//if (V >= A)
			//	{
			//	pGS->projLeft = cx - V/A*dx/2.f;
			//	pGS->projRight = cx + V/A*dx/2.f;
			//	pGS->projBottom = cy - dy/2.f;
			//	pGS->projTop = cy + dy/2.f;
			//	pGS->projNear = -1.1f*dz;
			//	pGS->projFar = 1.1f*dz;
			//	}
			//else
			//	{
			//	pGS->projLeft = cx - dx/2.f;
			//	pGS->projRight = cx + dx/2.f;
			//	pGS->projBottom = cy -A/V*dy/2.f;
			//	pGS->projTop = cy + A/V*dy/2.f;
			//	pGS->projNear = -1.1f * dz;
			//	pGS->projFar = 1.1f * dz;
			//	}
			

			//pGS->projLeft = cx - dx / 2.f;
			//pGS->projRight = cx + dx / 2.f;
			//pGS->projBottom = cy - dy / 2.f;
			//pGS->projTop = cy + dy / 2.f;
			//pGS->projNear = -1000.f;// cz - dz;
			//pGS->projFar = 1000.f;// cz + dz;

			//camera.Translate(glm::vec3(cx, cy, cz));
			//float dx = x1 - x0;
			//float dy = y1 - y0;
			//float dz = z1 - z0;

			//auto cameraPosInViewMat = camera.GetView() * glm::vec4(camera.GetEye(), 1.f);
			//glm::vec3 newPos = glm::inverse(camera.GetView())*glm::vec4((x0 + x1) / 2.f, (y0 + y1) / 2.f, (z0 + z1) / 2.f, 1.f);
			//
			//if (dx > dy)
			//	pGS->zoom = dx/pGS->windowWidth;
			//else
			//	pGS->zoom = (dy*pGS->windowWidth/pGS->windowHeight)/pGS->windowWidth;

			pGS->fit = false;
			}

		for (int i = 0; i < 10; ++i)
			{
			glm::mat4 cameraModel = glm::mat4 (1.f);
			cameraModel = glm::translate (cameraModel, camera.GetEye ());
			glm::mat4 cameraView = camera.GetView () * cameraModel;

			glm::mat4 model = glm::mat4 (1.f);
			model = glm::translate (model, cubePositions[i]);
			//if (i % 3 == 0)
			//	model = glm::rotate (model, i*(float)glfwGetTime ()-i * glm::radians (50.0f), glm::vec3 (0.5f, 1.0f, 0.0f));

			glm::mat4 view = camera.GetView ();// glm::mat4 (1.f);

			auto pos = camera.GetView () * glm::vec4(camera.GetEye (), 1.f);
			auto view1 = camera.GetView () * glm::vec4(camera.GetViewDir (), 1.f);
			//auto cameraPos = glm::vec3 (0.f, 0.f, 6.f);
			//glm::mat4 newView = glm::rotate<float> (view, glfwGetTime (), glm::vec3 (0.f, 1.f, 0.f));
			//cameraPos = newView * glm::vec4(cameraPos, 1.f);
			//auto viewDir = glm::normalize(cubePositions[0] - cameraPos);
			//auto lookAt = cameraPos + viewDir;
			//view = glm::lookAt (cameraPos, cubePositions[0], glm::vec3 (0.f, 1.f, 0.f));//glm::translate (view, glm::vec3 (0.f, 0.f, -3.f - 4*globalState.mixPercentage));

			//glm::mat4 projection = glm::perspective (glm::radians (45.f), 800.f / 600.f, 0.1f, 100.f);
			glm::mat4 projection = glm::ortho(pGS->projLeft, pGS->projRight, pGS->projBottom, pGS->projTop, pGS->projNear, pGS->projFar);

			glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "model"), 1, GL_FALSE, glm::value_ptr (model));
			glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "view"), 1, GL_FALSE, glm::value_ptr (view));
			glUniformMatrix4fv (glGetUniformLocation (shader.GetProgramId (), "projection"), 1, GL_FALSE, glm::value_ptr (projection));

			//glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays (GL_TRIANGLES, 0, 36);
			}

		glfwSwapBuffers (window);
		glfwPollEvents ();
		}

	// Clean up and exit
	glfwTerminate ();

	return 0;
	}

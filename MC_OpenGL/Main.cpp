#include <array>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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


class VAO
    {
    public:
        VAO(const std::vector<float> &data, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
            :   m_Data(data),
                m_Index(index),
                m_Size(size),
                m_Type(type),
                m_Normalized(normalized),
                m_Stride(stride),
                m_Pointer(pointer),
                m_Vao(0),
                m_Vbo(0)
            {
            glGenVertexArrays (1, &m_Vao);
            glBindVertexArray (m_Vao);

            glGenBuffers (1, &m_Vbo);
            glBindBuffer (GL_ARRAY_BUFFER, m_Vbo);
            glBufferData (GL_ARRAY_BUFFER, m_Data.size ()*sizeof (float), m_Data.data (), GL_STATIC_DRAW);

            glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray (0);
            }

        auto Bind () const -> void
            {
            glBindVertexArray (m_Vao);
            }

        auto GetNumVertices () const -> GLsizei
            {
            return m_Data.size ()/m_Stride;
            }


    private:
        std::vector<float>  m_Data;
        GLuint              m_Index;
        GLboolean           m_Normalized;
        const void*         m_Pointer;
        GLint               m_Size;
        GLsizei             m_Stride;
        GLenum              m_Type;
        GLuint              m_Vao;
        GLuint              m_Vbo;
    };


class Drawable
    {
    public:
        Drawable ()
            :   m_Vao(0),
                m_Vbo(0)
            {
            }

        virtual auto Draw () const -> void = 0;

    protected:
        GLuint m_Vao;
        GLuint m_Vbo;
    };


class Triangle : public Drawable
{
public:
    Triangle(const std::array<float, 9> &vertices)
        :   Drawable()
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
        glDrawArrays (GL_TRIANGLES, 0, 3);        
        }
};


}

/// <summary> Create a Window to display graphics on. </summary>
///
/// <param name="window"> [in,out] [in,out] A reference to the pointer to the window. Passing as reference
/// 					  because the pointer will be modified. </param>
///
/// <returns> A MC_OpenGL::ErrorCode. </returns>
auto GLFWInit (GLFWwindow* &window) -> MC_OpenGL::ErrorCode
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
    glfwSetKeyCallback(window, MC_OpenGL::GlfwCallbackKey);
    glfwSetWindowUserPointer (window, reinterpret_cast<void *>(&globalState));

    return MC_OpenGL::ErrorCode::NO_ERROR;
    }


int main()
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

	std::array<float, 9> vertices = { 
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f, 
         0.0f,  0.5f, 0.0f 
    };

    MC_OpenGL::Triangle triangle(vertices);

    const char *vertexShaderSource =    "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "void main ()\n"
                                        "{\n"
                                        "gl_Position = vec4 (aPos.x, aPos.y, aPos.z, 1.0);\n"
                                        "}\0";

    GLuint ID_vertexShader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (ID_vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader (ID_vertexShader);
    GLint success;
    const int infoLogSize = 512;
    GLchar infoLog[infoLogSize];
    glGetShaderiv (ID_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
        {
        glGetShaderInfoLog (ID_vertexShader, infoLogSize, nullptr, infoLog);
        std::cerr << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
        return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_VERTEX_SHADER_COMPILATION_FAILED);
        }

    const char *fragmentShaderSource =  "#version 330 core\n"
                                        "out vec4 FragColor;\n"
                                        "void main ()\n"
                                        "{\n"
                                        "FragColor = vec4 (1.0f, 0.5f, 0.2f, 1.0f);\n"
                                        "}\0";
    GLuint ID_fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (ID_fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader (ID_fragmentShader);
    glGetShaderiv (ID_fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
        {
        glGetShaderInfoLog (ID_fragmentShader, infoLogSize, nullptr, infoLog);
        std::cerr << "Error: Fragment shader compilation failed\n" << infoLog << std::endl;
        return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_FRAGMENT_SHADER_COMPILATION_FAILED);
        }

    GLuint ID_shaderProgram = glCreateProgram ();
    glAttachShader (ID_shaderProgram, ID_vertexShader);
    glAttachShader (ID_shaderProgram, ID_fragmentShader);
    glLinkProgram (ID_shaderProgram);
    glGetShaderiv (ID_shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
        {
        glGetProgramInfoLog (ID_shaderProgram, infoLogSize, nullptr, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
        return static_cast<int>(MC_OpenGL::ErrorCode::ERROR_SHADER_PROGRAM_LINKING_FAILED);
        }

    glDeleteShader (ID_vertexShader);
    glDeleteShader (ID_fragmentShader);

    // Game loop
    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram (ID_shaderProgram);

        
        triangle.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up and exit
    glfwTerminate();

    return 0;
}

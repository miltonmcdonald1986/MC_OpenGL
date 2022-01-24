#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLFWCallbackFunctions.h"


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
    ERROR_VERTEX_SHADER_COMPILATION_FAILED
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





	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
		};

    GLuint VAO;
    glGenVertexArrays (1, &VAO);
    glBindVertexArray (VAO);

    GLuint ID_vbo;
    glGenBuffers (1, &ID_vbo);

    glBindBuffer (GL_ARRAY_BUFFER, ID_vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

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

    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3*sizeof (float), (void *)0);
    glEnableVertexAttribArray (0);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram (ID_shaderProgram);
        glBindVertexArray (VAO);
        glDrawArrays (GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up and exit
    glfwTerminate();

    return 0;
}

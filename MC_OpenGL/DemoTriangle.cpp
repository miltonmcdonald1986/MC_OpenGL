#include "DemoTriangle.h"


MC_OpenGL::DemoTriangle::DemoTriangle ()
{
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";
	unsigned int vertexShader = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader (vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv (vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
		{
		glGetShaderInfoLog (vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	// fragment shader
	unsigned int fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader (fragmentShader);
	// check for shader compile errors
	glGetShaderiv (fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
		{
		glGetShaderInfoLog (fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	// link shaders
	m_ShaderProgram = glCreateProgram ();
	glAttachShader (m_ShaderProgram, vertexShader);
	glAttachShader (m_ShaderProgram, fragmentShader);
	glLinkProgram (m_ShaderProgram);
	// check for linking errors
	glGetProgramiv (m_ShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
		{
		glGetProgramInfoLog (m_ShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
	glDeleteShader (vertexShader);
	glDeleteShader (fragmentShader);

	float vertices[] = {
	   -0.5f, -0.5f, 0.0f, // left  
		0.5f, -0.5f, 0.0f, // right 
		0.0f,  0.5f, 0.0f  // top   
		};

	unsigned int VBO;
	glGenVertexArrays (1, &m_VAO);
	glGenBuffers (1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray (m_VAO);

	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *)0);
	glEnableVertexAttribArray (0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer (GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray (0);
}


auto MC_OpenGL::DemoTriangle::Draw () const -> void
{
	glUseProgram (m_ShaderProgram);
	glBindVertexArray (m_VAO);
	glDrawArrays (GL_TRIANGLES, 0, 3);		
}

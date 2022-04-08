#pragma once


#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <string>

#include "ErrorCode.h"


namespace MC_OpenGL {


class Shader
	{
	public:
		Shader ()
			{
			// TODO: Provide a default shader.
			}

		Shader (const std::string &vsFilename, const std::string &fsFilename)
			: m_ErrorCode (MC_OpenGL::ErrorCode::NONE)
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
			const char *vsSourceCode = strVsSourceCode.c_str ();
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
			const char *fsSourceCode = strFsSourceCode.c_str ();
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
			return m_ErrorCode == MC_OpenGL::ErrorCode::NONE;
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

		auto SetVec3(const std::string& name, const glm::vec3& value) const -> void
		{
			glUniform3fv(glGetUniformLocation(m_Id, name.c_str()), 1, &value[0]);
		}

	private:
		GLuint      m_Id = 0;
		ErrorCode	m_ErrorCode = ErrorCode::NONE;
		std::string m_InfoLog = "";
	};


}

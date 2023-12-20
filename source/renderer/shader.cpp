#include "shader.hpp"

#include "cmake_defines.hpp"
#include "gldebug.hpp"
#include "core/logger.hpp"
#include "utils.hpp"

#include <GL/glew.h>
#include <fstream>
#include <iterator>

Shader::Shader(const std::string& vs_path, const std::string &gs_path, const std::string &fs_path)
{
	m_is_valid = true;
	uint32_t vs = 0, gs = 0, fs = 0;

	// compiles shaders
	if (vs_path != "")
		vs = compile_shader(vs_path, GL_VERTEX_SHADER);
	
	if (gs_path != "")
		gs = compile_shader(gs_path, GL_GEOMETRY_SHADER);
	
	if (fs_path != "")
		fs = compile_shader(fs_path, GL_FRAGMENT_SHADER);

	// create program and link shaders
	m_id = glCreateProgram();

	if (vs)
		glAttachShader(m_id, vs);

	if (gs)
		glAttachShader(m_id, gs);

	if (fs)
		glAttachShader(m_id, fs);

	glLinkProgram(m_id);

	// error handling
	int success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		char info[512];
		glGetProgramInfoLog(m_id, 512, nullptr, info);
		SD_ERROR("Impossible de lier les shaders: ", info);
		m_is_valid = false;
	}

	// cleaning
	if (vs)
		glDeleteShader(vs);
	if (gs)
		glDeleteShader(gs);
	if (fs)
		glDeleteShader(fs);

}

Shader::~Shader()
{
	glDeleteProgram(m_id);
}

void Shader::bind()
{
	if (!m_is_valid)
		throw std::runtime_error("Impossible d'utiliser un shader non valide");
	else
		glUseProgram(m_id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::set_int(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_vec2(const std::string& name, float x, float y)
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string& name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

void Shader::set_mat4(const std::string& name, float* matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, matrix);
}

uint32_t Shader::compile_shader(const std::string& path, uint32_t type)
{
	std::string source = get_file_content( std::string{ROOT_DIR} + "/" + path );
	auto c_str_source = source.c_str();

	if( source == "" )
	{
		SD_ERROR("Impossible de récuperer le shader: ", path );
		return 0;
	}

	uint32_t shader = glCreateShader(type);
	glShaderSource(shader, 1, &c_str_source, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infos[512];
		glGetShaderInfoLog(shader, 512, nullptr, infos);
		SD_ERROR("Impossible de compiler le shader: ", infos);
		m_is_valid = false;
		return 0;
	}

	return shader;
}

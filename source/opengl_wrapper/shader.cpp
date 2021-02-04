#include "shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <iostream>

#include "../cmake_defines.hpp"

Shader::Shader( const std::string &vs_path, const std::string &fs_path ):
	m_is_valid(true) {

	// get shaders id
	unsigned vertex_shader = 0, fragment_shader = 0;

	try {
		vertex_shader   = compileShader( ROOT_DIR + vs_path, GL_VERTEX_SHADER   );
		fragment_shader = compileShader( ROOT_DIR + fs_path, GL_FRAGMENT_SHADER );

	} catch( std::runtime_error &e ) {
		m_is_valid = false;
		std::cout << e.what() << std::endl;
	}

	// create program and link fs and vs 
	m_id = glCreateProgram();

	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);

	glLinkProgram(m_id);

	// check for errors
	int success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if( !success ) m_is_valid = false;


	// free memory
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

Shader::~Shader() {
	glDeleteProgram(m_id);
}


unsigned Shader::compileShader( const std::string &path, unsigned type ) {

	// open the file
	std::fstream file { path , std::ios::in };
	file << std::noskipws;

	if( !file )
		throw std::runtime_error( "can't find " + path );
	
	// put the file into a string
	std::string std_str_src { std::istream_iterator<char>(file), std::istream_iterator<char>() };
	const char *src = std_str_src.c_str();

	file.close();

	// create an opengl shadre, give it the source code
	unsigned id = glCreateShader(type);
	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);


	// check for errors
	int result = 1;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if( !result ) {
		int length;

		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* msg = new char[length];
		glGetShaderInfoLog(id, length, &length, msg);
		std::string error_msg = msg;

		delete[] msg;
		glDeleteShader(id);
		
		throw std::runtime_error( "GLSL error (can't compile shader): " + error_msg );
	}

	return id;

} 

void Shader::use() {
	glUseProgram(m_id);
}

void Shader::setInt( const std::string &name, int value ) {
	glUniform1i( glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setMat4( const std::string &name, float *value) {
	glUniformMatrix4fv( glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, value);
}

void Shader::setVec3( const std::string &name, float x, float y, float z) {
	glUniform3f( glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

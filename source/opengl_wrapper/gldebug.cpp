#include "gldebug.hpp"

#include "gl_functions.hpp"
#include <iostream>

void _logGlErrors(int line, const char* file) {
	GLenum error_flag = GL_NO_ERROR;
	while((error_flag = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error in file: " << file << " at line" << line << ": ";
		switch(error_flag) {
			case GL_INVALID_ENUM:
				std::cout << "GL_INVALID_ENUM" << std::endl;
				break;
			case GL_INVALID_VALUE:
				std::cout << "GL_INVALID_VALUE" << std::endl;
				break;
			case GL_INVALID_OPERATION:
				std::cout << "GL_INVALID_OPERATION" << std::endl;
				break;
			case GL_OUT_OF_MEMORY:
				std::cout << "GL_OUT_OF_MEMORY" << std::endl;
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
				break;
			default: break;
		}
	}
}

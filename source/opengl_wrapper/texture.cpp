#include "texture.hpp"

#include <glad/glad.h>
#include <stb_image.h>

#include "../cmake_defines.hpp"

Texture::Texture( const std::string &path ):
	m_is_valid(true) {

	// generate and bind the texture
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	// set texture's parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, channel_number;
	unsigned char *image_data = stbi_load( (ROOT_DIR + path).c_str(), &width, &height, &channel_number, 0);

	if( image_data ) {

		unsigned color_format = GL_RGB;
		if(channel_number == 1)
			color_format = GL_RED;
		if(channel_number == 4)
			color_format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else // handle error
		m_is_valid = false;

	// free image data
	stbi_image_free(image_data);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_id);
}
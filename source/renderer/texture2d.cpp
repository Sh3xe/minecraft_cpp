#include "texture2d.hpp"

#include <glad/glad.h>
#include <stdexcept>

#include "cmake_defines.hpp"
#include "stb_image.h"
#include "core/logger.hpp"

bool Texture2D::load_from_file( const std::string &path ) {

	int32_t w, h, channels;
	// TODO: régler le problème de chemin
	uint8_t* image_data = stbi_load( (std::string{ROOT_DIR} + "/" + path).c_str(), &w, &h, &channels, 0);

	m_is_ok = true;

	if (image_data) {
		bind();

		//TODO: add functions for this
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		uint32_t format = GL_GREEN;
		if (channels == 3) format = GL_RGB;
		else if (channels == 4) format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		m_is_ok = true;
	}
	else {
		m_is_ok = false;
		SD_ERROR("Can't open texture ", (std::string{ROOT_DIR} + "/res/" + path));
	}

	stbi_image_free(image_data);
	unbind();

	return m_is_ok;
}

void Texture2D::bind() const{

	if( !m_is_ok )
		throw std::runtime_error("Tentative de liaison d'une texture invalide.");
	else
		glBindTexture( GL_TEXTURE_2D, m_id );

}

void Texture2D::unbind() const{
	glBindTexture( GL_TEXTURE_2D, 0 );
}
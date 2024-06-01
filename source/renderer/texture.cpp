#include "texture.hpp"
#include <glad/glad.h>

Texture::Texture() {
	glGenTextures(1, &m_id);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}
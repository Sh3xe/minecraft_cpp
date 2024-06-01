#include "texture.hpp"
#include <glad/glad.h>

Texture::Texture() {
	glGenTextures(1, &m_id);
}
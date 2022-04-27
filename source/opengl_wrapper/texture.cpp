#include "texture.hpp"
#include <GL/glew.h>

Texture::Texture() {
	glGenTextures(1, &m_id);
}
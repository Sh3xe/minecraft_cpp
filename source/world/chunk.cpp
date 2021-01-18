/*
#include "chunk.hpp"
#include <glad/glad.h>

Chunk::Chunk() {
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
}

void Chunk::setBlock(int x, int y, int z, char type) {
	m_block_data[x][y][z] = type;
	m_should_update = true;
}

char Chunk::getBlock(int x, int y, int z) {
	return m_block_data[x][y][z];
}

void Chunk::generateMesh() {
	glBindVertexArray(m_vao);

	// blablabla

	m_should_update = false;
}

void Chunk::draw() {
	if(m_should_update)
		generateMesh();
	
	if(m_element_count) {
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, m_element_count);
	}
}
*/
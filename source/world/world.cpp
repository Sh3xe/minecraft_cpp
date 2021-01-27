#include "world.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include "../core/camera.hpp"
#include "block.hpp"
#include <iostream>

World::World():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl"),
	m_tilset("resources/images/tileset.png") {
	
	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(3.141592853f / 2.f, 16.f / 9.f, .01f, 100.f);

	m_shader.use();
	m_shader.setMat4("projection", glm::value_ptr(projection_matrix));

	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			m_chunks[i][j] = Chunk(i * 16, 0, j * 16, this);
}

void World::setBlock(int x, int y, int z, unsigned char type) {
	int chunk_x = x / CHUNK_X,
		chunk_z = z / CHUNK_Z;

	int coord_x = x % CHUNK_X,
		coord_y = y % CHUNK_Y,
		coord_z = z % CHUNK_Z;

	return m_chunks[chunk_x][chunk_z].setBlock(coord_x, coord_y, coord_z, type);
}

unsigned char World::getBlock(int x, int y, int z) {
	if (x >= CHUNK_X * 4 || x < 0 || y >= CHUNK_Y || y < 0 || z >= CHUNK_Z * 4 || z < 0)
		return Blocks::AIR;

	int chunk_x = x / CHUNK_X,
		chunk_z = z / CHUNK_Z;

	int coord_x = x % CHUNK_X,
		coord_y = y % CHUNK_Y,
		coord_z = z % CHUNK_Z;

	return m_chunks[chunk_x][chunk_z].getBlock(coord_x, coord_y, coord_z);
}

void World::draw( Camera &camera ) {
	m_shader.use();

	glm::mat4 view_matrix = camera.getViewMatrix();
	m_shader.setMat4("view", glm::value_ptr(view_matrix));

	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			m_chunks[i][j].draw(camera, m_tilset, m_shader);
}
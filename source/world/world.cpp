#include "world.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include "../core/camera.hpp"
#include "block.hpp"
#include <iostream>

World::World():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl"),
	m_tilset("resources/images/tileset.png"),
	m_noise_generator(2546245)  {
	
	// set shader uniforms
	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(3.141592853f / 2.0f, 16.0f / 9.0f, 0.01f, 128.0f);

	m_shader.use();
	m_shader.setMat4("projection", glm::value_ptr(projection_matrix));

	// construct chunks
	for(int i = 0; i < WORLD_X; ++i)
		for (int j = 0; j < WORLD_Z; ++j) {
			// insert a unique_ptr<Chunk> at [i * 16, j * 16] location, and get an iterator to the inserted element
			auto it = m_chunks.insert( std::pair< std::pair<int, int>, Chunk*>(
				std::pair<int, int>(i * 16, j * 16), new Chunk(i * 16, j * 16)
			)).first;

			// use the iterator to set neighbours chunks and generate terrain
			it->second->generateTerrain(m_noise_generator);
		}

	// give each chunk its neighbours
	for (int i = 0; i < WORLD_X; ++i)
		for (int j = 0; j < WORLD_Z; ++j) {
			// px = +x, mx = -x, pz = +z, mz = -z
			Chunk* px = nullptr, * mx = nullptr, * pz = nullptr, * mz = nullptr;

			// get a reference to neighbours chunks
			if (i > 0)
				mx = m_chunks[std::pair<int, int>((i - 1) * 16, j * 16)];
			if (i != WORLD_X - 1)
				px = m_chunks[std::pair<int, int>((i + 1) * 16, j * 16)];
			if (j > 0)
				mz = m_chunks[std::pair<int, int>(i * 16, (j - 1) * 16)];
			if (j != WORLD_Z - 1)
				pz = m_chunks[std::pair<int, int>(i * 16, (j + 1) * 16)];
				//;

			// use the iterator to set neighbours chunks and generate terrain
			m_chunks[ std::pair<int, int>(i * 16, j * 16) ]->setNeighbours(px, mx, pz, mz);

		}

	getBlock(0, 0, 0);
}

World::~World(){
	for (auto& chunk : m_chunks)
		delete chunk.second;
}

void World::setBlock(int x, int y, int z, unsigned char type) {
	if (x >= CHUNK_X * WORLD_X || x < 0 || y >= CHUNK_Y || y < 0 || z >= CHUNK_Z * WORLD_Z || z < 0)
		return;

	int chunk_x = x / CHUNK_X,
		chunk_z = z / CHUNK_Z;

	int coord_x = x % CHUNK_X,
		coord_z = z % CHUNK_Z;
	
	m_chunks[ std::pair<int, int>(chunk_x * 16, chunk_z * 16) ]->setBlock(coord_x, y, coord_z, type);
}

unsigned char World::getBlock(int x, int y, int z) {
	if (x >= CHUNK_X * WORLD_X || x < 0 || y >= CHUNK_Y || y < 0 || z >= CHUNK_Z * WORLD_Z || z < 0)
		return Blocks::AIR;

	int chunk_x = x / CHUNK_X,
		chunk_z = z / CHUNK_Z;

	int coord_x = x % CHUNK_X,
		coord_z = z % CHUNK_Z;

	return m_chunks[std::pair<int, int>(chunk_x * 16, chunk_z * 16)]->getBlock(coord_x, y, coord_z);
}

void World::draw( Camera &camera ){
	m_shader.use();

	glm::mat4 view_matrix = camera.getViewMatrix();
	m_shader.setMat4("view", glm::value_ptr(view_matrix));

	for(int i = 0; i < WORLD_X; ++i)
		for(int j = 0; j < WORLD_Z; ++j)
			m_chunks[std::pair<int, int>(i * 16, j * 16)]->draw(camera, m_tilset, m_shader);
}
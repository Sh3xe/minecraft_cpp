#include "world.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include "../core/camera.hpp"
#include "block.hpp"

World::World():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl"),
	m_tilset("resources/images/tileset.png"),
	m_noise_generator(0)  {
	
	// set shader uniforms
	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(3.141592853f / 2.0f, 16.0f / 9.0f, 0.01f, 128.0f);

	m_shader.use();
	m_shader.setMat4("projection", glm::value_ptr(projection_matrix));

	// construct chunks
	for(int i = 0; i < WORLD_X; ++i)
		for (int j = 0; j < WORLD_Z; ++j) {
			// insert a unique_ptr<Chunk> at [i * 16, j * 16] location, and get an iterator to the inserted element
			auto it = m_chunks.insert( std::pair< std::pair<int, int>, std::unique_ptr<Chunk>>(
				std::pair<int, int>(i * 16, j * 16), std::unique_ptr<Chunk>(new Chunk(i * 16, j * 16))
			)).first;

			// use the iterator to set neighbours chunks and generate terrain
			it->second->generateTerrain(m_noise_generator);
		}

	updateChunksNeighbours();
}

World::~World(){
	m_chunks.clear();
}

void World::updateChunksNeighbours() {
	for(auto it = m_chunks.begin(); it != m_chunks.end(); ++it) {
		// set all pointers to nullptr
		Chunk* px = nullptr, * mx = nullptr, * pz = nullptr, * mz = nullptr;

		// try to find the -x neighbour, if found, set "mx" to it
		auto neighbour = m_chunks.find( std::pair<int, int>(it->first.first - 16, it->first.second) );
		if (neighbour != m_chunks.end())
			mx = neighbour->second.get();
		// same for +x
		neighbour = m_chunks.find( std::pair<int, int>(it->first.first + 16, it->first.second) );
		if(neighbour != m_chunks.end())
			px = neighbour->second.get();
		// same for -y
		neighbour = m_chunks.find( std::pair<int, int>(it->first.first, it->first.second - 16) );
		if(neighbour != m_chunks.end())
			mz = neighbour->second.get();
		// same for +y
		neighbour = m_chunks.find( std::pair<int, int>(it->first.first, it->first.second + 16) );
		if(neighbour != m_chunks.end())
			pz = neighbour->second.get();

		// set the neighbours chunk for this current chunk
		it->second->setNeighbours(px, mx, pz, mz);
	}
}

void World::setBlock(int x, int y, int z, unsigned char type) {
	int chunk_x = x / CHUNK_X,
		chunk_z = z / CHUNK_Z;

	int coord_x = x % CHUNK_X,
		coord_z = z % CHUNK_Z;
	
	auto chunk = m_chunks.find(std::pair<int, int>(chunk_x * 16, chunk_z * 16));
	if (chunk != m_chunks.end() && y < CHUNK_Y && y >= 0)
		chunk->second->setBlock(coord_x, y, coord_z, type);
}

unsigned char World::getBlock(int x, int y, int z) {
	int chunk_x = x / CHUNK_X,
		chunk_z = z / CHUNK_Z;

	int coord_x = x % CHUNK_X,
		coord_z = z % CHUNK_Z;

	auto chunk = m_chunks.find(std::pair<int, int>(chunk_x * 16, chunk_z * 16));
	if(chunk != m_chunks.end() && y < CHUNK_Y && y >= 0)
		return chunk->second->getBlock(coord_x, y, coord_z);
	return Blocks::AIR;
}

void World::update( double delta_time, Camera &camera ) {
	// calculate the position of the chunk ( / 16 )
	glm::ivec3 camera_position = camera.getPosition();
	int chunk_x = ( static_cast<int>(camera_position.x) / CHUNK_X );
	int chunk_z = ( static_cast<int>(camera_position.z) / CHUNK_Z );

	bool chunk_changed = false;
	// remove far away chunks
	for(auto chunk = m_chunks.begin(); chunk != m_chunks.end();) {
		glm::ivec2 chunk_pos = chunk->second->getPosition();
		if( // if the cunk is too far
			chunk_pos.x > (chunk_x + 3) * 16 ||
			chunk_pos.x < (chunk_x - 3) * 16 ||
			chunk_pos.y > (chunk_z + 3) * 16 ||
			chunk_pos.y < (chunk_z - 3) * 16
			) {
			// delete, erase
			chunk = m_chunks.erase(chunk);
			chunk_changed = true;
		}
		else
			++chunk;
	}

	if (chunk_changed) {
		// add a chunk if needed
		for (int i = chunk_x - 3; i <= chunk_x + 3; ++i)
			for (int j = chunk_z - 3; j <= chunk_z + 3; ++j) {
				auto it = m_chunks.find(std::pair<int, int>(i * 16, j * 16));
				if (it == m_chunks.end()) {
					auto new_chunk = m_chunks.insert( std::pair< std::pair<int, int>, std::unique_ptr<Chunk>>(
						std::pair<int, int>(i * 16, j * 16), std::unique_ptr<Chunk>(new Chunk(i * 16, j * 16))
					)).first;
					
					new_chunk->second->generateTerrain(m_noise_generator);
				}
			}

		updateChunksNeighbours();
	}

}

void World::draw( Camera &camera ){
	m_shader.use();

	glm::mat4 view_matrix = camera.getViewMatrix();
	m_shader.setMat4("view", glm::value_ptr(view_matrix));

	for(auto &chunk: m_chunks)
		chunk.second->draw(camera, m_tilset, m_shader);
}
#include "chunk.hpp"

#include <random>
#include "gl_functions.hpp"

#include "world.hpp"
#include "../core/timer.hpp"
#include "../core/camera.hpp"

#include "../opengl_wrapper/vertex.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"

Chunk::Chunk(int x, int z):
	m_position(x, z),
	m_neighbours({nullptr, nullptr , nullptr , nullptr }),
	m_mesh(x, z) {

	// fill blocks array
	for(int x = 0; x < CHUNK_X; ++x)
	for(int y = 0; y < CHUNK_Y; ++y)
	for(int z = 0; z < CHUNK_Z; ++z)
		m_block_data[ x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y ] = Blocks::AIR;
}

Chunk::Chunk() :
	Chunk(0, 0) {
}

Chunk::~Chunk() {
}

void Chunk::setPosition(int x, int z) {
	m_position.x = x;
	m_position.y = z;
}

void Chunk::setNeighbours(Chunk *px, Chunk *mx, Chunk *pz, Chunk *mz) {
	// set neighbour's chunks (used for block checking)
	m_neighbours[0] = px;
	m_neighbours[1] = mx;
	m_neighbours[2] = pz;
	m_neighbours[3] = mz;
}

void Chunk::set_block(int x, int y, int z, unsigned char type) {

	if (x >= CHUNK_X && m_neighbours[0] != nullptr)
		m_neighbours[0]->set_block(0, y, z, type);
	if (x < 0 && m_neighbours[1] != nullptr)
		m_neighbours[1]->set_block(CHUNK_X - 1, y, z, type);
	if (z >= CHUNK_Z && m_neighbours[2] != nullptr)
		m_neighbours[2]->set_block(x, y, 0, type);
	if (z < 0 && m_neighbours[3] != nullptr)
		m_neighbours[3]->set_block(x, y, CHUNK_Z - 1, type);

	if (x >= 0 && x < CHUNK_X && y >= 0 && y < CHUNK_Y && z >= 0 && z < CHUNK_Z) {
		m_block_data[x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y] = type;
		m_should_update = true;

		
		if (x == CHUNK_X - 1 && m_neighbours[0] != nullptr) m_neighbours[0]->m_should_update = true;
		if (x == 0 && m_neighbours[1] != nullptr) m_neighbours[1]->m_should_update = true;
		if (z == CHUNK_Z - 1 && m_neighbours[2] != nullptr) m_neighbours[2]->m_should_update = true;
		if (z == 0 && m_neighbours[3] != nullptr) m_neighbours[3]->m_should_update = true;
	}
}

unsigned char Chunk::set_block(int x, int y, int z) {
	// warning: only works for 1 block out-of-range.
	// if x, y, z are out of range, we return the right block in the neighbour's chunk
	if(y >= CHUNK_Y)
		return Blocks::AIR;
	if(y < 0)
		return Blocks::STONE;
	if(x >= CHUNK_X)
		return m_neighbours[0] != nullptr ? m_neighbours[0]->set_block(0, y, z): Blocks::STONE;
	if(x < 0)
		return m_neighbours[1] != nullptr ? m_neighbours[1]->set_block(CHUNK_X - 1, y, z): Blocks::STONE;
	if(z >= CHUNK_Z)
		return m_neighbours[2] != nullptr ? m_neighbours[2]->set_block(x, y, 0): Blocks::STONE;
	if(z < 0)
		return m_neighbours[3] != nullptr ? m_neighbours[3]->set_block(x, y, CHUNK_Z - 1): Blocks::STONE;

	// return the block in the current chunk
	return m_block_data[x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y];
}

uint8_t Chunk::fast_get(int x, int y, int z)
{
	return m_block_data[x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y];
}

void Chunk::fast_set(int x, int y, int z, uint8_t block )
{
	m_block_data[x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y] = block;
}

void Chunk::generateMesh()
{
	m_mesh.clear();

	for(int x = 0; x < CHUNK_X; ++x)
	for(int z = 0; z < CHUNK_Z; ++z)
	for(int y = 0; y < CHUNK_Y; ++y) {
		// adds a face the the mesh if needed

		Blocks::BlockType type = (Blocks::BlockType)set_block(x, y, z);
		if(type == Blocks::AIR) continue;
	
		// +x
		if (!set_block(x + 1, y, z)  )
			m_mesh.add_face(x, y, z, Directions::PX, type);
		// -x
		if (!set_block(x - 1, y, z) )
			m_mesh.add_face(x, y, z, Directions::MX, type);
		// +y
		if (!set_block(x, y + 1, z) )
			m_mesh.add_face(x, y, z, Directions::PY, type);
		// -y
		if (!set_block(x, y - 1, z) )
			m_mesh.add_face(x, y, z, Directions::MY, type);
		// +z
		if (!set_block(x, y, z + 1) )
			m_mesh.add_face(x, y, z, Directions::PZ, type);
		// -z
		if (!set_block(x, y, z - 1) )
			m_mesh.add_face(x, y, z, Directions::MZ, type);
	}
	
	m_mesh.send_to_gpu();
	m_should_update = false;
}

void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader ) {
	if( m_should_update )
		generateMesh();
	
	m_mesh.render(camera, tileset, shader);
}

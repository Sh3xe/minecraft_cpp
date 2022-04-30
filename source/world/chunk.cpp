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
	BlockID air_block = BlockDB::get().name_get("air").id;

	for(int x = 0; x < CHUNK_X; ++x)
	for(int y = 0; y < CHUNK_Y; ++y)
	for(int z = 0; z < CHUNK_Z; ++z)
		m_block_data[ x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y ] = air_block;
}

Chunk::Chunk() :
	Chunk(0, 0) {
}

Chunk::~Chunk() {
}

void Chunk::set_position(int x, int z) {
	m_position.x = x;
	m_position.y = z;
}

void Chunk::set_neighbours(Chunk *px, Chunk *mx, Chunk *pz, Chunk *mz) {
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

BlockID Chunk::get_block(int x, int y, int z) {
	// warning: only works for 1 block out-of-range.
	// if x, y, z are out of range, we return the right block in the neighbour's chunk

	auto &db = BlockDB::get();

	if(y >= CHUNK_Y)
		return db.name_get("air").id;
	if(y < 0)
		return db.name_get("stone").id;;
	if(x >= CHUNK_X)
		return m_neighbours[0] != nullptr ? m_neighbours[0]->get_block(0, y, z): db.name_get("stone").id;
	if(x < 0)
		return m_neighbours[1] != nullptr ? m_neighbours[1]->get_block(CHUNK_X - 1, y, z): db.name_get("stone").id;
	if(z >= CHUNK_Z)
		return m_neighbours[2] != nullptr ? m_neighbours[2]->get_block(x, y, 0): db.name_get("stone").id;
	if(z < 0)
		return m_neighbours[3] != nullptr ? m_neighbours[3]->get_block(x, y, CHUNK_Z - 1): db.name_get("stone").id;

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

void Chunk::generate_mesh()
{
	m_mesh.clear();

	for(int x = 0; x < CHUNK_X; ++x)
	for(int z = 0; z < CHUNK_Z; ++z)
	for(int y = 0; y < CHUNK_Y; ++y) {
		// adds a face the the mesh if needed

		//__TODO
		auto id = get_block(x, y, z);
		if( id == BlockDB::get().name_get("air").id ) continue;
	
		// +x
		if ( !get_block(x + 1, y, z)  )
			m_mesh.add_face(x, y, z, Directions::PX, id);
		// -x
		if ( !get_block(x - 1, y, z) )
			m_mesh.add_face(x, y, z, Directions::MX, id);
		// +y
		if ( !get_block(x, y + 1, z) )
			m_mesh.add_face(x, y, z, Directions::PY, id);
		// -y
		if ( !get_block(x, y - 1, z) )
			m_mesh.add_face(x, y, z, Directions::MY, id);
		// +z
		if ( !get_block(x, y, z + 1) )
			m_mesh.add_face(x, y, z, Directions::PZ, id);
		// -z
		if ( !get_block(x, y, z - 1) )
			m_mesh.add_face(x, y, z, Directions::MZ, id);
	}
	
	m_mesh.send_to_gpu();
	m_should_update = false;
}

void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader ) {
	if( m_should_update )
		generate_mesh();
	
	m_mesh.render(camera, tileset, shader);
}



std::pair<int, int> get_pos_inside_chunk( int x, int z )
{
    // get the coordinate of x, y, z inside this chunk
    int coord_x = (x) % CHUNK_X,
        coord_z = (z) % CHUNK_Z;
    
    if( (z) < 0 ) // handle the case when z is negative
        coord_z += CHUNK_Z - 1;

	if( (x) < 0 ) // handle the case when z is negative
        coord_x += CHUNK_X - 1;
    
	return {coord_x, coord_z};
}

std::pair<int, int> get_pos_of_chunk( int x, int z )
{
	// the the coordinates of the chunk where x, y, z is located
    int chunk_x = (x) / CHUNK_X,
        chunk_z = (z) / CHUNK_Z;
	
	if( (x) < 0 ) // handle the case when x is negative
        --chunk_x;

	if( (z) < 0 ) // handle the case when z is negative
        --chunk_z;

	return {chunk_x, chunk_z};
}
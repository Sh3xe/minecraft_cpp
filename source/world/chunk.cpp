#include "chunk.hpp"

#include <cassert>
#include <random>
#include <cmath>
#include "gl_functions.hpp"

#include "world.hpp"
#include "../core/timer.hpp"
#include "../core/camera.hpp"

#include "../renderer/vertex.hpp"
#include "../renderer/shader.hpp"
#include "../renderer/texture.hpp"

Chunk::Chunk( BlockDB &db, int x, int z):
	m_position(x, z),
	m_db( &db ),
	m_neighbours({nullptr, nullptr , nullptr , nullptr })
{
	// créer 3 maillages: un pour les blocs solides, un pour les blocs transparents, un pour la végétation
	for( int i = 0; i < 3; ++i )
	{
		m_meshes[i].x = x;
		m_meshes[i].y = z;
	}

	m_meshes[1].transparency = true;
	m_meshes[2].face_culling = false;

	// remplit le tronçon par de l'air
	BlockID air_block = m_db->id_from_name("air");

	for(int x = 0; x < CHUNK_X; ++x)
	for(int y = 0; y < CHUNK_Y; ++y)
	for(int z = 0; z < CHUNK_Z; ++z)
		m_block_data[ x + y * CHUNK_X + z * CHUNK_X * CHUNK_Y ] = air_block;
}

Chunk::~Chunk()
{
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

void Chunk::set_block(int x, int y, int z, BlockID type) {

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

BlockID Chunk::get_block(int x, int y, int z)
{
	// warning: only works for 1 block out-of-range.
	// if x, y, z are out of range, we return the right block in the neighbour's chunk

	if(y >= CHUNK_Y)
		return m_db->id_from_name("air");
	if(y < 0)
		return m_db->id_from_name("stone");
	if(x >= CHUNK_X)
		return m_neighbours[0] != nullptr ? m_neighbours[0]->get_block(0, y, z): m_db->id_from_name("stone");
	if(x < 0)
		return m_neighbours[1] != nullptr ? m_neighbours[1]->get_block(CHUNK_X - 1, y, z): m_db->id_from_name("stone");
	if(z >= CHUNK_Z)
		return m_neighbours[2] != nullptr ? m_neighbours[2]->get_block(x, y, 0): m_db->id_from_name("stone");
	if(z < 0)
		return m_neighbours[3] != nullptr ? m_neighbours[3]->get_block(x, y, CHUNK_Z - 1): m_db->id_from_name("stone");

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
	for( int i = 0; i < 3; ++i )
		m_meshes[i].clear();

	for(int x = 0; x < CHUNK_X; ++x)
	for(int z = 0; z < CHUNK_Z; ++z)
	for(int y = 0; y < CHUNK_Y; ++y)
	{
		// adds a face the the mesh if needed
		auto block = m_db->get_block( get_block(x, y, z) );

		if( block.id == m_db->id_from_name("air") ) continue;
	
		BlockType blk;

		// +x
		blk = m_db->get_block( get_block(x + 1, y, z) );
		if ( !blk.visible || blk.mesh_group != block.mesh_group || blk.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::px, block);
		// -x
		blk = m_db->get_block( get_block(x - 1, y, z) );
		if ( !blk.visible || blk.mesh_group != block.mesh_group || blk.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::mx, block);
		// +y
		blk = m_db->get_block( get_block(x, y + 1, z) );
		if ( !blk.visible || blk.mesh_group != block.mesh_group || blk.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::py, block);
		// -y
		blk = m_db->get_block( get_block(x, y - 1, z) );
		if ( !blk.visible || blk.mesh_group != block.mesh_group || blk.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::my, block);
		// +z
		blk = m_db->get_block( get_block(x, y, z + 1) );
		if ( !blk.visible || blk.mesh_group != block.mesh_group || blk.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::pz, block);
		// -z
		blk = m_db->get_block( get_block(x, y, z - 1) );
		if ( !blk.visible || blk.mesh_group != block.mesh_group || blk.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::mz, block);
	}
	
	for( int i = 0; i < 3; ++i )
		m_meshes[i].send_to_gpu();
	m_should_update = false;
}

void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader )
{
	auto cam_pos = camera.get_position();

	if( m_should_update )
		generate_mesh();

	for( int i = 0; i < 3; ++i )
	{
		if( m_meshes[i].transparency )
			m_meshes[i].sort_faces_from_distance( cam_pos );
		m_meshes[i].render( tileset, shader );
	}
}

std::pair<int, int> get_pos_inside_chunk( int x, int z )
{
	int coord_x = (x < 0) ?
		((x+1) % CHUNK_X) + CHUNK_X - 1: x%CHUNK_X;

	int coord_z = (z < 0) ?
		((z+1) % CHUNK_Z) + CHUNK_Z - 1: z%CHUNK_Z;

	return {coord_x, coord_z};
}

std::pair<int, int> get_pos_of_chunk( int x, int z )
{
	int chunk_x = (x < 0) ?
		floor( x / (float)CHUNK_X): x/CHUNK_X;

	int chunk_z = (z < 0) ?
		floor( z / (float)CHUNK_Z): z/CHUNK_Z;

	return {chunk_x, chunk_z};
}
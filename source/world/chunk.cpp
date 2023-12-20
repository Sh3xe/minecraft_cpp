#include "chunk.hpp"

#include <cassert>
#include <random>
#include <cmath>
#include "gl_functions.hpp"

#include "world.hpp"
#include "core/timer.hpp"
#include "core/camera.hpp"

#include "renderer/vertex.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"

Chunk::Chunk( int x, int z):
	m_position(x, z),
	m_neighbours({nullptr, nullptr , nullptr , nullptr })
{
	// creates 3 meshes: one solid for the blocks, one for water and one for foliages
	for( int i = 0; i < 3; ++i )
	{
		m_meshes[i].x = x;
		m_meshes[i].y = z;
	}

	m_meshes[1].transparency = true;
	m_meshes[2].cull = false;

	// fills the chunks with air blocks
	for(int x = 0; x < CHUNK_SIDE; ++x)
	for(int y = 0; y < CHUNK_HEIGHT; ++y)
	for(int z = 0; z < CHUNK_SIDE; ++z)
		m_block_data[ x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT ] = blk::BlockType::air;
}

Chunk::~Chunk()
{
}

void Chunk::set_position(int x, int z)
{
	m_position.x = x;
	m_position.y = z;
}

void Chunk::set_neighbours(Chunk *px, Chunk *mx, Chunk *pz, Chunk *mz)
{
	// set neighbour's chunks (used for block checking)
	m_neighbours[0] = px;
	m_neighbours[1] = mx;
	m_neighbours[2] = pz;
	m_neighbours[3] = mz;
}

void Chunk::set_block(int x, int y, int z, blk::BlockType type)
{
	assert( x >= 0 && x < CHUNK_SIDE && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_SIDE );

	// if we set a block that's not air
	// mark this layer as not empty
	if( type != blk::BlockType::air )
		m_layers[y] = true;

	// set the block
	m_block_data[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT] = type;
	if( state != ChunkState::need_generation )
		state = ChunkState::need_mesh_update;

	// notify the neighbours
	if (x == CHUNK_SIDE - 1 && m_neighbours[0] != nullptr && m_neighbours[0]->state != ChunkState::need_generation )
		m_neighbours[0]->state = ChunkState::need_mesh_update;
	if (x == 0 && m_neighbours[1] != nullptr && m_neighbours[1]->state != ChunkState::need_generation)
		m_neighbours[1]->state = ChunkState::need_mesh_update;
	if (z == CHUNK_SIDE - 1 && m_neighbours[2] != nullptr && m_neighbours[2]->state != ChunkState::need_generation)
		m_neighbours[2]->state = ChunkState::need_mesh_update;
	if (z == 0 && m_neighbours[3] != nullptr  && m_neighbours[3]->state != ChunkState::need_generation)
		m_neighbours[3]->state = ChunkState::need_mesh_update;
	
}

blk::BlockType Chunk::get_block(int x, int y, int z)
{
	// warning: only works for 1 block out-of-range.
	// if x, y, z are out of range, we return the right block in the neighbour's chunk

	if(y >= CHUNK_HEIGHT)
		return blk::BlockType::air;
	if(y < 0)
		return blk::BlockType::stone;
	if(x >= CHUNK_SIDE)
		return m_neighbours[0] != nullptr ? m_neighbours[0]->get_block(0, y, z): blk::BlockType::stone;
	if(x < 0)
		return m_neighbours[1] != nullptr ? m_neighbours[1]->get_block(CHUNK_SIDE - 1, y, z): blk::BlockType::stone;
	if(z >= CHUNK_SIDE)
		return m_neighbours[2] != nullptr ? m_neighbours[2]->get_block(x, y, 0): blk::BlockType::stone;
	if(z < 0)
		return m_neighbours[3] != nullptr ? m_neighbours[3]->get_block(x, y, CHUNK_SIDE - 1): blk::BlockType::stone;

	// return the block in the current chunk
	return m_block_data[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT];
}

blk::BlockType Chunk::fast_get(int x, int y, int z)
{
	return m_block_data[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT];
}

void Chunk::fast_set(int x, int y, int z, blk::BlockType block )
{
	m_block_data[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT] = block;
}

void Chunk::generate_mesh()
{
	for( int i = 0; i < 3; ++i )
		m_meshes[i].clear();

	for(int x = 0; x < CHUNK_SIDE; ++x)
	for(int z = 0; z < CHUNK_SIDE; ++z)
	for(int y = 0; y < CHUNK_HEIGHT; ++y)
	{
		if( !m_layers[y] ) continue;
		
		// ajoute une face si besoin
		blk::BlockType block_type = get_block(x, y, z);
		if( block_type == blk::BlockType::air ) continue;

		auto block = blk::get_block( block_type );

		if( block.shape == blk::BlockShape::x )
		{
			m_meshes[block.mesh_group].add_x_shape(x, y, z, block);
			continue;
		}

		blk::BlockData blk_data;

		// +x
		blk_data = blk::get_block( get_block(x + 1, y, z) );
		if ( !blk_data.visible || blk_data.mesh_group != block.mesh_group || blk_data.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::px, block);
		// -x
		blk_data = blk::get_block( get_block(x - 1, y, z) );
		if ( !blk_data.visible || blk_data.mesh_group != block.mesh_group || blk_data.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::mx, block);
		// +y
		blk_data = blk::get_block( get_block(x, y + 1, z) );
		if ( !blk_data.visible || blk_data.mesh_group != block.mesh_group || blk_data.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::py, block);
		// -y
		blk_data = blk::get_block( get_block(x, y - 1, z) );
		if ( !blk_data.visible || blk_data.mesh_group != block.mesh_group || blk_data.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::my, block);
		// +z
		blk_data = blk::get_block( get_block(x, y, z + 1) );
		if ( !blk_data.visible || blk_data.mesh_group != block.mesh_group || blk_data.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::pz, block);
		// -z
		blk_data = blk::get_block( get_block(x, y, z - 1) );
		if ( !blk_data.visible || blk_data.mesh_group != block.mesh_group || blk_data.shape != block.shape )
			m_meshes[block.mesh_group].add_face(x, y, z, Directions::mz, block);
	}
	
	state = ChunkState::need_upload;
}

void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader )
{
	for( int i = 0; i < 3; ++i )
	{
		if( m_meshes[i].transparency )
			m_meshes[i].sort_faces_from_distance( camera.position );
		m_meshes[i].render( tileset, shader );
	}
}

std::pair<int, int> get_pos_inside_chunk( int x, int z )
{
	int coord_x = (x < 0) ?
		((x+1) % CHUNK_SIDE) + CHUNK_SIDE - 1: x%CHUNK_SIDE;

	int coord_z = (z < 0) ?
		((z+1) % CHUNK_SIDE) + CHUNK_SIDE - 1: z%CHUNK_SIDE;

	return {coord_x, coord_z};
}

std::pair<int, int> get_pos_of_chunk( int x, int z )
{
	int chunk_x = (x < 0) ?
		floor( x / (float)CHUNK_SIDE): x/CHUNK_SIDE;

	int chunk_z = (z < 0) ?
		floor( z / (float)CHUNK_SIDE): z/CHUNK_SIDE;

	return {chunk_x, chunk_z};
}
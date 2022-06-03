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

// ChunkData member functions
void ChunkData::set_block(int x, int y, int z, blk::BlockType type)
{
	assert( x >= 0 && x < CHUNK_SIDE && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_SIDE );

	// if we set a block that's visible
	// mark this layer as not empty
	if( blk::get_block(type).visible )
		layers[y] = true;

	// set the block
	blocks[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT] = type;
	if( state != ChunkState::need_generation )
		state = ChunkState::need_mesh_update;

	// notify the neighbours
	if (x == CHUNK_SIDE - 1 && neighbours[0] != nullptr && neighbours[0]->state != ChunkState::need_generation )
		neighbours[0]->state = ChunkState::need_mesh_update;
	if (x == 0 && neighbours[1] != nullptr && neighbours[1]->state != ChunkState::need_generation)
		neighbours[1]->state = ChunkState::need_mesh_update;
	if (z == CHUNK_SIDE - 1 && neighbours[2] != nullptr && neighbours[2]->state != ChunkState::need_generation)
		neighbours[2]->state = ChunkState::need_mesh_update;
	if (z == 0 && neighbours[3] != nullptr  && neighbours[3]->state != ChunkState::need_generation)
		neighbours[3]->state = ChunkState::need_mesh_update;
}

blk::BlockType ChunkData::get_block(int x, int y, int z)
{
	// warning: only works for 1 block out-of-range.
	// if x, y, z are out of range, we return the right block in the neighbour's chunk

	if(y >= CHUNK_HEIGHT)
		return blk::BlockType::air;
	if(y < 0)
		return blk::BlockType::stone;
	if(x >= CHUNK_SIDE)
		return neighbours[0] != nullptr ? neighbours[0]->get_block(0, y, z): blk::BlockType::stone;
	if(x < 0)
		return neighbours[1] != nullptr ? neighbours[1]->get_block(CHUNK_SIDE - 1, y, z): blk::BlockType::stone;
	if(z >= CHUNK_SIDE)
		return neighbours[2] != nullptr ? neighbours[2]->get_block(x, y, 0): blk::BlockType::stone;
	if(z < 0)
		return neighbours[3] != nullptr ? neighbours[3]->get_block(x, y, CHUNK_SIDE - 1): blk::BlockType::stone;

	// return the block in the current chunk
	return blocks[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT];
}

blk::BlockType ChunkData::fast_get(int x, int y, int z)
{
	return blocks[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT];
}

void ChunkData::fast_set(int x, int y, int z, blk::BlockType block)
{
	blocks[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT] = block;
}

// non member functions
void fill_chunk_mesh( ChunkData &data )
{
	data.block_mesh.clear();
	data.water_mesh.clear();
	data.foliage_mesh.clear();

	for(int y = 0; y < CHUNK_HEIGHT; ++y)
	{
		// is this layer is marked as empty, don't do anything
		if( !data.layers[y] ) continue;

		for(int x = 0; x < CHUNK_SIDE; ++x)
		for(int z = 0; z < CHUNK_SIDE; ++z)
		{	
			// only add faces if the block is visible
			blk::BlockType block_type = data.get_block(x, y, z);
			if( !blk::get_block(block_type).visible ) continue;

			auto block = blk::get_block( block_type );
			std::array< blk::BlockType, 6> neighbour_blocks;

			neighbour_blocks[0] = data.get_block(x + 1, y, z);
			neighbour_blocks[1] = data.get_block(x - 1, y, z);
			neighbour_blocks[2] = data.get_block(x, y + 1, z);
			neighbour_blocks[3] = data.get_block(x, y - 1, z);
			neighbour_blocks[4] = data.get_block(x, y, z + 1);
			neighbour_blocks[5] = data.get_block(x, y, z - 1);

			// for each direction, add face if needed
			for( uint8_t d = 0; d != static_cast<uint8_t>(Direction::mz); ++d )
			{
				auto other = blk::get_block( neighbour_blocks[d] );
				if( !other.visible || other.shape != block.shape ) continue;

				// add the face to the correct mesh
				if( block.mesh_group == blk::MeshGroup::block && other.mesh_group != block.mesh_group )
					add_face( data.block_mesh, static_cast<Direction>(d), block );
				else if( block.mesh_group == blk::MeshGroup::water && other.mesh_group != block.mesh_group )
					add_face( data.water_mesh, static_cast<Direction>(d), block );
				else if( block.mesh_group == blk::MeshGroup::foliage && other.mesh_group != block.mesh_group )
					add_face( data.foliage_mesh, static_cast<Direction>(d), block );
					
			}
		}
	}
	
	data.state = ChunkState::need_upload;
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

void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader )
{
	for( int i = 0; i < 3; ++i )
	{
		if( m_meshes[i].transparency )
			m_meshes[i].sort_faces_from_distance( camera.position );
		m_meshes[i].render( tileset, shader );
	}
}


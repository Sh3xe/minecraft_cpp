#include "chunk.hpp"

#include <cassert>
#include <random>
#include <cmath>
#include <glad/glad.h>

#include "world.hpp"
#include "core/timer.hpp"
#include "core/camera.hpp"
#include "core/logger.hpp"

#include "graphics/vertex.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

// TODO: ajouter des aserts partout sur les get set... 

// ChunkData member functions
void Chunk::set_block(int x, int y, int z, blk::BlockType type)
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

blk::BlockType Chunk::get_block(int x, int y, int z)
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

blk::BlockType Chunk::fast_get(int x, int y, int z)
{
	return blocks[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT];
}

void Chunk::fast_set(int x, int y, int z, blk::BlockType block)
{
	blocks[x + y * CHUNK_SIDE + z * CHUNK_SIDE * CHUNK_HEIGHT] = block;
}


// non member functions
void Chunk::construct_mesh()
{
	// sera utilisé dans la boucle pour stocker les blocs voisins
	// au bloc courrant
	std::array< blk::BlockType, 6> neighbour_blocks;

	for(int y = 0; y < CHUNK_HEIGHT; ++y)
	{
		// si cette couche est vide, on ne la traite pas
		if( !layers[y] ) continue;

		for(int x = 0; x < CHUNK_SIDE; ++x)
		for(int z = 0; z < CHUNK_SIDE; ++z)
		{	
			// si le bloc n'est pas visible, on ne le traite pas
			blk::BlockType block_type = fast_get(x, y, z);
			if( !blk::get_block(block_type).visible ) continue;

			auto block = blk::get_block( block_type );

			//TODO: ajouter la forme mode #
			if( block.shape != blk::BlockShape::cubic )
			{
				mesh.add_x_shape(x, y, z, block);
				continue;
			}

			neighbour_blocks[0] = get_block(x + 1, y, z);
			neighbour_blocks[1] = get_block(x - 1, y, z);
			neighbour_blocks[2] = get_block(x, y + 1, z);
			neighbour_blocks[3] = get_block(x, y - 1, z);
			neighbour_blocks[4] = get_block(x, y, z + 1);
			neighbour_blocks[5] = get_block(x, y, z - 1);

			// pour chaques directions, on ajoute une face si besoin
			for( uint8_t d = 0; d != static_cast<uint8_t>(Direction::mz); ++d )
			{
				auto other = blk::get_block( neighbour_blocks[d] );
				// si cette face est cachée, pas besoin de l'ajouter
				if( other.visible && other.shape == block.shape ) continue;

				mesh.add_face( x, y, z, static_cast<Direction>(d), block );	
			}
		}
	}
	state = ChunkState::need_upload;
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

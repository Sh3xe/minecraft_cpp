#include "terrain_generator.hpp"
#include "core/logger.hpp"
#include "utils.hpp"
#include "core/timer.hpp"
#include "world.hpp"
#include "world/blocks.hpp"

#include <cassert>
#include <cmath>

static constexpr float s1{ 0.01f };
static constexpr float s2{ 0.05f };
static constexpr float s3{ 0.005f };

/* math curves / functions */

/**
 * @brief Get the sky stencil value (0 = no sky island here, 1 = sky island can spawn here)
 */
float get_sky_stencil_multiply( float noise_value )
{
	constexpr float threshold = 0.5f; 
	noise_value = (noise_value + 1.0f) / 2.0f;

	if(noise_value < threshold)
		return 0.0f;
	return (noise_value - threshold) / (1.0f - threshold);
}

/**
 * @brief get a stencil value that ranges between 0 and 1 for the sky islands
 * 
 * @param val_01 height of the block, normalized (0 = bottom of sky, 1 = top of sky, linear)
 * @return float between 0 and 2 to be multiplied by the normalized noise value, then maped to [-1,+whatever positive value]
 */
float get_sky_stencil_y_multiply( float val_01 )
{
	constexpr float island_bottom = 0.7f;
	constexpr float island_top = 0.9f;

	if(val_01 < island_bottom)
		return val_01 / island_bottom;
	if(val_01 < island_top)
		return 1.0;
	return 1.0f - (val_01-island_top) / (1.0f-island_top);
}

float get_caves_transitions( int y, float min, float max )
{
	float Y = y - min;
	float delta = max - min;

	if( Y < 5.0f ) return Y / 5.0f;
	if( Y > delta - 5.0f ) return (Y - delta) / (-5.0f);
	return 1.0f;
}

float get_3d_multiplier( int y )
{
	constexpr float ed = 8.0f;
	if (y > ed ) return 0.25f;
	return 0.25f + (y / (ed) * 0.2f);
}

float map_ocean_value( float val_01 )
{
	constexpr float st = 0.05f;
	constexpr float ed = 0.4f;
	if( val_01 < st ) return 0.0f;
	if( val_01 > ed ) return 1.0f;
	return (val_01 - st) / (ed - st);
}

float ocean_floor_add( int y )
{
	if( y > 7 ) return 0.0f;
	float val = 1.0f - (y / 7.0f);
	return val * val;
}

float get_tree_density( int y, float min, float max )
{
	return 1.0f - (y - min) / (max - min);
}

blk::BlockType TerrainGenerator::get_ore_type( int y )
{
	static int ore_type = 0;
	ore_type = ++ore_type % 10;

	static blk::BlockType ores[] = {
		blk::BlockType::coal_ore,
		blk::BlockType::iron_ore,
		blk::BlockType::gold_ore,
		blk::BlockType::diamond_ore
	};

	int max_ore = 4 - (y / 100) * 4;
	
	return ores[ ore_type % max_ore ];
}


/* constructors / methods */

TerrainGenerator::TerrainGenerator( World *world ):
	m_world( world )
{
}

Chunk &TerrainGenerator::generate( Chunk &chunk )
{
	for( int y = 0; y < CHUNK_HEIGHT; ++y )
		chunk.m_layers[y] = false;

	for( int x = 0; x < CHUNK_SIDE; ++x )
	{	
		for( int z = 0; z < CHUNK_SIDE; ++z )
		{
			int X = chunk.m_position.x + x;
			int Z = chunk.m_position.y + z;

			m_mask[x][z] = to_01(m_noise.noise(X * s3, Z*s3), -1, 1);
			m_height[x][z] = to_01(m_noise.noise(X * s1, 0, Z * s1), -1, 1);
			m_ocean[x][z] = map_ocean_value( to_01(m_noise.noise((X + 2.2114f) * s3, (Z - 2.2114f)*s3*0.95f), -1, 1) );
			
			shape_underworld( chunk, x, z );
			shape_caves( chunk, x, z );
			shape_surface( chunk, x, z );
			shape_sky( chunk, x, z );

			paint_underworld( chunk, x, z );
			paint_caves( chunk, x, z );
			paint_surface( chunk, x, z );
			paint_sky( chunk, x, z );
		}
	}

	for( int i = 0; i < 4; ++i )
	{
		if( chunk.m_neighbours[i] != nullptr && chunk.m_neighbours[i]->state != ChunkState::need_generation )
			chunk.m_neighbours[i]->state = ChunkState::need_mesh_update;
	}

	chunk.state = ChunkState::need_mesh_update;
	return chunk;
}

void TerrainGenerator::push_structure( blk::StructType struct_id, int px, int py, int pz )
{
	const auto &structure = get_struct( struct_id );

	px -= structure.center_x;
	py -= structure.center_y;
	pz -= structure.center_z;

	for( int x = 0; x < structure.x_length; ++x )
	for( int z = 0; z < structure.z_length; ++z )
	{
		auto [chunk_x, chunk_z] = get_pos_of_chunk(px+x, pz+z);
		auto [coord_x, coord_z] = get_pos_inside_chunk(px+x, pz+z);

		auto &block_vector = m_world->m_chunk_blocks[{chunk_x*CHUNK_SIDE, chunk_z*CHUNK_SIDE}];

		for( int y = 0; y < structure.y_length && (py + y < CHUNK_HEIGHT); ++y )
		{
			ToBePlaced block_info (
				coord_x,
				py + y,
				coord_z,
				structure.get(x, y, z)
			);

			block_vector.push_back( block_info );
		}
	}
}

void TerrainGenerator::load_structs()
{
	m_trees[0] = blk::StructType::smalltree1;
	m_trees[1] = blk::StructType::smalltree2;
	m_trees[2] = blk::StructType::smalltree3;
	m_trees[3] = blk::StructType::mediumtree1;
	m_trees[4] = blk::StructType::mediumtree2;
	m_trees[5] = blk::StructType::mediumtree3;
	m_trees[6] = blk::StructType::bigtree1;
	m_trees[7] = blk::StructType::bigtree2;
	m_trees[8] = blk::StructType::bigtree3;

	m_flowers[0] = blk::BlockType::bluesmallflower;
	m_flowers[1] = blk::BlockType::blueflower;
	m_flowers[2] = blk::BlockType::purplesmallflower;
	m_flowers[3] = blk::BlockType::purpleflower;
	m_flowers[4] = blk::BlockType::xgrass;
	m_flowers[5] = blk::BlockType::smalltree;
}

void TerrainGenerator::add_ore( Chunk &chunk, int x, int y, int z, blk::BlockType ore )
{
	for( int i = x - 1; i <= x + 1; ++i )
	for( int j = y - 1; j <= y + 1; ++j )
	for( int k = z - 1; k <= z + 1; ++k )
	{
		if( i >= 0 && j >= 0 && k >= 0
			&& i < CHUNK_SIDE && j < CHUNK_HEIGHT && k < CHUNK_SIDE
			&& chunk.fast_get(i, j, k) == blk::BlockType::stone )
		{
			chunk.fast_set( i, j, k, ore );
		}
	}
}

/* création forme */

void TerrainGenerator::shape_underworld( Chunk &chunk, int x, int z )
{

}

void TerrainGenerator::shape_caves( Chunk &chunk, int x, int z )
{
	float X = x + chunk.m_position.x;
	float Z = z + chunk.m_position.y;

	for( int y = m_caverns_min; y < m_caverns_max; y++ )
	{
		float v = to_01(m_noise.noise( X * s2, y * s2 * 1.5f, Z * s2 ), 1, -1) * get_caves_transitions( y, m_caverns_min, m_caverns_max ) * 0.7f;
		v -= 0.5f;
		if( v < 0 )
		{
			chunk.fast_set(x, y, z, blk::BlockType::stone );
			chunk.m_layers[y] = true;
		}
	}
}

void TerrainGenerator::shape_surface( Chunk &chunk, int x, int z )
{
	const float delta_h = m_surface_max - m_surface_min;

	float X = x + chunk.m_position.x;
	float Z = z + chunk.m_position.y;

	for( int y = m_ocean_bottom; y < m_surface_max; y++ )
	{
		float Y = 0;

		if( y >= m_surface_min )
			Y = (y - m_surface_min);

		float v = m_height[x][z]*m_mask[x][z] - (Y / delta_h);
		v += m_noise.noise( X * s2, y * s2, Z * s2 ) * m_mask[x][z] * get_3d_multiplier(y - m_ocean_bottom);
		v -= m_ocean[x][z] * 0.3f;
		v += ocean_floor_add( y - m_ocean_bottom );

		if( v > 0 )
			chunk.fast_set(x, y, z, blk::BlockType::stone );
	}
}

void TerrainGenerator::shape_sky( Chunk &chunk, int x, int z )
{
	float X = (float)x + chunk.m_position.x;
	float Z = (float)z + chunk.m_position.y;

	// general shape of the islands in the sky
	float stencil_noise = m_noise.noise(X * s1, 0.0f, Z * s1);
	float stencil = get_sky_stencil_multiply(stencil_noise);

	for(int y = m_sky_min; y < m_sky_max; ++y) 
	{
		// details of the terrain
		float noise_value = m_noise.noise(X * s2, y * s2, Z * s2);
		float y_stencil = get_sky_stencil_y_multiply((float)(y-m_sky_min)/(float)(m_sky_max-m_sky_min));
		noise_value = to_01(noise_value, -1.0f, 1.0f);
		noise_value = map(noise_value*noise_value*stencil*y_stencil*2.0f, 0.0f, 1.0f, -1.0f, 1.0f);

		if(noise_value > 0.0f)
		{
			chunk.m_layers[y] = true;
			chunk.fast_set(x, y, z, blk::BlockType::stone);
		}
	}
}

/* filling blocks */

void TerrainGenerator::paint_underworld( Chunk &chunk, int x, int z )
{

}

void TerrainGenerator::paint_caves( Chunk &chunk, int x, int z )
{
	if( x < 1 || x >= CHUNK_SIDE - 1 ||
		z < 1 || z >= CHUNK_SIDE - 1 )
		return;

	// minerais

	static std::bernoulli_distribution d { 0.002 };

	for( int y = m_caverns_min; y < m_caverns_max; y++ )
	{
		if( d(m_rd) )
			add_ore( chunk, x, y, z, get_ore_type(y) );
	}
}

void TerrainGenerator::paint_surface( Chunk &chunk, int x, int z )
{
	// initialization of randow distributions
	static std::bernoulli_distribution flower_gen { 0.1f };
	static std::bernoulli_distribution rd_50 { 0.5f };
	static std::bernoulli_distribution ore_rd { 0.004f };
	static int tree_type = 0;
	static int flower_type = 0;
	
	// max non-empty layer
	int layer_max = 0;

	// calculates world cordinates
	float X = x + chunk.m_position.x;
	float Z = z + chunk.m_position.y;

	int depth = 0;
	for( int y = m_surface_max; y >= m_ocean_bottom; --y )
	{
		auto block = chunk.fast_get(x, y, z);

		if( block != blk::BlockType::air )
		{
			++depth;
			chunk.m_layers[y] = true;
		}
		else
			depth = 0;

		if( y <= m_water_level )
		{
			if( block == blk::BlockType::air && y != m_water_level )
			{
				chunk.fast_set( x, y, z, blk::BlockType::water );
				chunk.m_layers[y] = true;
			}

			else if( depth >= 1 && depth < 4 )
			{
				chunk.m_layers[y] = true;
				std::bernoulli_distribution d { 1.0f - (m_water_level - y) / 10.0f };
				if( d( m_rd ) )
					chunk.fast_set( x, y, z, blk::BlockType::sand );
				else
					chunk.fast_set( x, y, z, blk::BlockType::gravel );
			}
				
			continue;
		}

		// if this block is just under an air block: put a grass block
		if( depth == 1 )
		{
			float tree_dens_value = get_tree_density(y, m_surface_min, m_surface_max);
			std::bernoulli_distribution tree_dis { tree_dens_value * 0.05f };

			chunk.fast_set( x, y, z, blk::BlockType::grass );

			if( tree_dis(m_rd) && y > m_water_level )
			{
				const auto &tree = m_trees[ tree_type % static_cast<int>( ceil(9 * tree_dens_value)) ];
				push_structure( tree, x + chunk.m_position.x, y+1, z + chunk.m_position.y );
				tree_type = (++tree_type) % 9;
			}

			if( y < CHUNK_HEIGHT && chunk.fast_get(x, y+1, z) == blk::BlockType::air )
			{
				if (flower_gen(m_rd))
				{
					chunk.fast_set(x, y+1, z, m_flowers[flower_type]);
				}
				if (flower_gen(m_rd))
				{
					chunk.fast_set( x, y+1, z, blk::BlockType::xgrass );
				}

				flower_type = (++flower_type) % 4;
			}
		}

		// the dirt on the other 3
		else if( depth > 1 && depth <= 4 )
			chunk.fast_set( x, y, z, blk::BlockType::dirt );
		else if( depth >= 5 && depth <= 6 && rd_50( m_rd ) )
			chunk.fast_set( x, y, z, blk::BlockType::dirt );

		if( ore_rd( m_rd ) )
			add_ore( chunk, x, y, z, blk::BlockType::coal_ore );

	}

}

void TerrainGenerator::paint_sky( Chunk &chunk, int x, int z )
{
	int dist_since_sky = 0;
	for(int y = m_sky_max-1; y >= m_sky_min; --y)
	{
		// calculate the depth of the block relative to the nearest surface
		if(chunk.fast_get(x, y, z) == blk::BlockType::air)
		{
			dist_since_sky = 0;
			continue;
		}
		else
		 	++dist_since_sky;

		if(dist_since_sky == 1)
			chunk.fast_set(x, y, z, blk::BlockType::grass);
		else if (dist_since_sky <= 3)
			chunk.fast_set(x, y, z, blk::BlockType::dirt);
	}
}
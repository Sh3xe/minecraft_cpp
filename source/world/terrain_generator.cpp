#include "terrain_generator.hpp"
#include "core/logger.hpp"
#include "utils.hpp"
#include "core/timer.hpp"
#include "world.hpp"

#include <cassert>
#include <cmath>
#include <random>

//static constexpr float s1{ 0.01f };
//static constexpr float s2{ 0.05f };
//static constexpr float s3{ 0.003f };

static constexpr float s1{ 0.01f };
static constexpr float s2{ 0.05f };
static constexpr float s3{ 0.003f };

TerrainGenerator::TerrainGenerator( BlockDB &db, World *world ):
	m_db( &db ),
	m_world( world )
{
}

Chunk &TerrainGenerator::generate( Chunk& chunk )
{

	for( int x = 0; x < CHUNK_SIDE; ++x )
	for( int z = 0; z < CHUNK_SIDE; ++z )
	{
		int X = chunk.m_position.x + x;
		int Z = chunk.m_position.y + z;

		m_mask[x][z] = to_01(m_noise.noise(X * s3, Z*s3), -1, 1);
		m_height[x][z] = to_01(m_noise.noise(X * s1, 0, Z * s1), -1, 1);

		make_shape( chunk, x, z );
		paint_blocks( chunk, x, z );

		/*
		shape_underworld( chunk, x, z );
		shape_caves( chunk, x, z );
		shape_surface( chunk, x, z );
		shape_sky( chunk, x, z );

		paint_underworld( chunk, x, z );
		paint_caves( chunk, x, z );
		paint_surface( chunk, x, z );
		paint_sky( chunk, x, z );
		*/
	}

	for( int i = 0; i < 4; ++i )
		if( chunk.m_neighbours[i] != nullptr && chunk.m_neighbours[i]->state != ChunkState::need_generation )
			chunk.m_neighbours[i]->state = ChunkState::need_mesh_update;

	chunk.state = ChunkState::need_mesh_update;
	return chunk;
}

void TerrainGenerator::make_shape( Chunk& chunk, int x, int z )
{
	const float delta_h = m_surface_max - m_surface_min;

	auto stone_id = m_db->id_from_name("stone");

	float X = x + chunk.m_position.x;
	float Z = z + chunk.m_position.y;

	for( int y = 0; y < m_surface_max; y++ )
	{
		float Y = 0;

		if( y >= m_surface_min )
			Y = (y - m_surface_min);

		float v = m_height[x][z]*m_mask[x][z] - (Y / delta_h);
		v += m_noise.noise( X * s2, y * s2, Z * s2 ) * 0.3f * m_mask[x][z];
		//v += m_noise.noise( X, y, Z, s2 ) * 0.3f * mask;

		if( v > 0 )
			chunk.fast_set(x, y, z, stone_id );
	}
}

void TerrainGenerator::paint_blocks( Chunk& chunk, int x, int z )
{
	// generateur aléatoire
	static std::default_random_engine e;
	std::bernoulli_distribution demi { 0.5f };

	// on met en cache certains id pour ne pas les re-récuperer dans une map
	BlockID air_id = m_db->id_from_name("air");
	BlockID grass_id = m_db->id_from_name("grass");
	BlockID dirt_id = m_db->id_from_name("dirt");
	BlockID sand_id = m_db->id_from_name("sand");
	BlockID water_id = m_db->id_from_name("water");
	auto &tree = m_db->get_struct("bigtree1");
	
	// couche maximale non vide du tronçon
	int layer_max = 0;

	// on récupère les coordonnées relatif au monde (vs relatif au tronçon)
	float X = x + chunk.m_position.x;
	float Z = z + chunk.m_position.y;

	std::bernoulli_distribution d { 0.01f };

	int depth = 0;
	for( int y = CHUNK_HEIGHT - 1; y >= 0; --y )
	{
		auto block = chunk.fast_get(x, y, z);

		if( block != air_id )
			++depth;
		else
			depth = 0;

		if( y <= water_level )
		{
			if( block == air_id && y != water_level )
				chunk.fast_set(x, y, z, water_id);

			else if( depth >= 1 && depth < 4 )
				chunk.fast_set(x, y, z, sand_id);
				
			continue;
		}

		// si on est juste en dessous d'un bloc d'air: on place du gazon
		if( depth == 1 )
		{
			chunk.fast_set( x, y, z, grass_id );
			if( d(e) && y > water_level )
				push_structure( tree,
					x + chunk.m_position.x - tree.center_x,
					y+1 - tree.center_y,
					z + chunk.m_position.y - tree.center_z );
		}

		// puis de la terre sur 3 autres blocs
		else if( depth > 1 && depth <= 4 )
			chunk.fast_set( x, y, z, dirt_id );
	}

}

void TerrainGenerator::push_structure( const Structure &structure, int px, int py, int pz )
{
	for( int x = 0; x < structure.x_length; ++x )
	for( int z = 0; z < structure.z_length; ++z )
	{
		auto [chunk_x, chunk_z] = get_pos_of_chunk(px+x, pz+z);
		auto [coord_x, coord_z] = get_pos_inside_chunk(px+x, pz+z);

		auto &block_vector = m_world->m_chunk_blocks[{chunk_x*CHUNK_SIDE, chunk_z*CHUNK_SIDE}];

		for( int y = 0; y < structure.y_length && (py + y < CHUNK_HEIGHT); ++y )
		{
			ToBePlaced block_info (coord_x, py + y, coord_z, structure.get(x, y, z));
			block_vector.push_back( block_info );
		}
	}
}
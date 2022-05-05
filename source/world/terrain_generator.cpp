#include "terrain_generator.hpp"
#include "core/logger.hpp"
#include "utils.hpp"
#include "core/timer.hpp"
#include "world.hpp"

#include <cassert>
#include <cmath>
#include <random>

static constexpr float s1{ 0.01f };
static constexpr float s2{ 0.05f };
static constexpr float s3{ 0.005f };

float get_caves_transitions( int y, float min, float max )
{
	float Y = y - min;
	float delta = max - min;

	if( Y < 10.0f ) return Y / 10.0f;
	if( Y > delta - 10.0f ) return (Y - delta) / (-10.0f);
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

TerrainGenerator::TerrainGenerator( BlockDB &db, World *world ):
	m_db( &db ),
	m_world( world )
{
}

Chunk &TerrainGenerator::generate( Chunk& chunk )
{
	//Timer timer;

	for( int x = 0; x < CHUNK_SIDE; ++x )
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

	for( int i = 0; i < 4; ++i )
		if( chunk.m_neighbours[i] != nullptr && chunk.m_neighbours[i]->state != ChunkState::need_generation )
			chunk.m_neighbours[i]->state = ChunkState::need_mesh_update;

	chunk.state = ChunkState::need_mesh_update;
	return chunk;
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


void TerrainGenerator::shape_underworld( Chunk &chunk, int x, int z )
{

}

void TerrainGenerator::shape_caves( Chunk &chunk, int x, int z )
{
	auto stone_id = m_db->id_from_name("stone");
	float X = x + chunk.m_position.x;
	float Z = z + chunk.m_position.y;

	for( int y = m_caverns_min; y < m_caverns_max; y++ )
	{
		float v = to_01(m_noise.noise( X * s2, y * s2, Z * s2 ), 1, -1) * get_caves_transitions( y, m_caverns_min, m_caverns_max ) * 0.7f;
		v -= 0.5f;
		if( v < 0 )
			chunk.fast_set(x, y, z, stone_id );
	}
}

void TerrainGenerator::shape_surface( Chunk &chunk, int x, int z )
{
	const float delta_h = m_surface_max - m_surface_min;

	auto stone_id = m_db->id_from_name("stone");

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
			chunk.fast_set(x, y, z, stone_id );
	}
}

void TerrainGenerator::shape_sky( Chunk &chunk, int x, int z )
{

}


void TerrainGenerator::paint_underworld( Chunk &chunk, int x, int z )
{

}

void TerrainGenerator::paint_caves( Chunk &chunk, int x, int z )
{

}

void TerrainGenerator::paint_surface( Chunk &chunk, int x, int z )
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
	for( int y = m_surface_max; y >= m_ocean_bottom; --y )
	{
		auto block = chunk.fast_get(x, y, z);

		if( block != air_id )
			++depth;
		else
			depth = 0;

		if( y <= m_water_level )
		{
			if( block == air_id && y != m_water_level )
				chunk.fast_set(x, y, z, water_id);

			else if( depth >= 1 && depth < 4 )
				chunk.fast_set(x, y, z, sand_id);
				
			continue;
		}

		// si on est juste en dessous d'un bloc d'air: on place du gazon
		if( depth == 1 )
		{
			chunk.fast_set( x, y, z, grass_id );
			if( d(e) && y > m_water_level )
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

void TerrainGenerator::paint_sky( Chunk &chunk, int x, int z )
{

}

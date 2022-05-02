#include "terrain_generator.hpp"
#include "chunk.hpp"
#include "core/logger.hpp"
#include "utils.hpp"

#include <cassert>
#include <random>

static constexpr float s1{ 0.01f };
static constexpr float s2{ 0.05f };
static constexpr float s3{ 0.003f };

TerrainGenerator::TerrainGenerator( BlockDB &db, ChunkToBePlace *chunk_blocks ):
	m_db( &db ),
	m_chunk_blocks( chunk_blocks )
{
}

Chunk &TerrainGenerator::generate( Chunk& chunk )
{
	make_shape(chunk);
	paint_blocks(chunk);

	for( auto *c: chunk.m_neighbours )
		if( c != nullptr && c->state != ChunkState::need_generation )
			c->state = ChunkState::need_mesh_update;

	chunk.state = ChunkState::need_mesh_update;
	return chunk;
}

void TerrainGenerator::make_shape( Chunk& chunk )
{
	const float delta_h = m_surface_max - m_surface_min;

	for( int x = 0; x < CHUNK_X; x++ )
	for( int z = 0; z < CHUNK_Z; z++ )
	{
		float X = x + chunk.m_position.x;
		float Z = z + chunk.m_position.y;

		const float mask = to_01(m_noise.noise(X * s3, Z*s3), -1, 1);
		const float value = to_01(m_noise.noise(X * s1, 0, Z * s1), -1, 1);

		for( int y = 0; y < CHUNK_Y; y++ )
		{
			float Y = 0;

			if( y >= m_surface_min )
				Y = (y - m_surface_min);

			float v = value*mask - (Y / delta_h);
			v += m_noise.noise( X * s2, y * s2, Z * s2 ) * 0.3f * mask;

			if( v > 0 )
				chunk.fast_set(x, y, z, m_db->id_from_name("stone") );
		}
	}
}

void TerrainGenerator::paint_blocks(Chunk& chunk)
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
	auto &tree = m_db->get_struct("tree");

	// pour chaque colonne du tronçon
	for( int x = 0; x < CHUNK_X; ++x )
	for( int z = 0; z < CHUNK_X; ++z )
	{
		// on récupère les coordonnées relatif au monde (vs relatif au tronçon)
		float X = x + chunk.m_position.x;
		float Z = z + chunk.m_position.y;

		// le même masque utilisé pour "aplatir" le terrain: + plat = + d'arbre
		const float mask = to_01(m_noise.noise(X*s3, Z*s3), -1, 1);
		std::bernoulli_distribution d { 0.04f * ( 1.0f - mask ) };

		int depth = 0;
		for( int y = CHUNK_Y - 1; y >= 0; --y )
		{
			auto block = chunk.fast_get(x, y, z);

			if( block != air_id ) ++depth;
			else depth = 0;

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
					push_structure( tree, x + chunk.m_position.x - (tree.x_length/2), y+1, z + chunk.m_position.y -(tree.z_length/2) );
			}

			// puis de la terre sur 3 autres blocs
			else if( depth > 1 && depth <= 4 )
				chunk.fast_set( x, y, z, dirt_id );
		}
	}
}

void TerrainGenerator::push_structure( const Structure &structure, int px, int py, int pz )
{
	for( int x = 0; x < structure.x_length; ++x )
	for( int z = 0; z < structure.z_length; ++z )
	{
		auto [chunk_x, chunk_z] = get_pos_of_chunk(px+x, pz+z);
		auto [coord_x, coord_z] = get_pos_inside_chunk(px+x, pz+z);

		auto &block_vector = (*m_chunk_blocks)[{chunk_x*CHUNK_X, chunk_z*CHUNK_Z}];

		for( int y = 0; y < structure.y_length && (py + y < CHUNK_Y); ++y )
		{
			ToBePlaced block_info (coord_x, py + y, coord_z, structure.get(x, y, z));
			if( block_info.block != m_db->id_from_name("air") )
				block_vector.push_back( block_info );
		}
	}
}
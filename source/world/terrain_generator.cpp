#include "terrain_generator.hpp"
#include "chunk.hpp"
#include "core/logger.hpp"
#include "utils.hpp"

#include <cassert>
#include <random>

TerrainGenerator::TerrainGenerator()
{
	if (!m_tree_struct.load_from_file("resources/blocks/tree.txt"))
		SD_ERROR("Impossible de charger la structure: 'tree.txt'");
}

Chunk &TerrainGenerator::generate(Chunk& chunk)
{
	make_shape(chunk);
	paint_blocks(chunk);
	place_blocks(chunk);

	chunk.m_should_update = true;
	return chunk;
}

void TerrainGenerator::make_shape(Chunk& chunk)
{ 
	constexpr float s1{ 0.01f };
	constexpr float s2{ 0.05f };
	constexpr float s3{ 0.003f };
	const float delta_h = m_surface_max - m_surface_min;

	for(int x = 0; x < CHUNK_X; x++)
	for(int z = 0; z < CHUNK_Z; z++)
	{
		float X = x + chunk.m_position.x;
		float Z = z + chunk.m_position.y;

		const float mask = to_01(m_noise.noise(X * s3, Z*s3), -1, 1);
		const float value = to_01(m_noise.fractal(3, X * s1, 0, Z * s1), -1, 1);

		for(int y = 0; y < CHUNK_Y; y++)
		{
			float Y = 0;

			if( y >= m_surface_min )
				Y = (y - m_surface_min);

			float v = value*mask - (Y / delta_h);
			v += m_noise.fractal(3, X * s2, y * s2, Z * s2 ) * 0.3f * mask;

			if(v > 0)
				chunk.fast_set(x, y, z, Blocks::STONE);
		}
	}
}

void TerrainGenerator::paint_blocks(Chunk& chunk)
{
	static std::bernoulli_distribution d { 0.005f };
	static std::default_random_engine e;

	for( int x = 0; x < CHUNK_X; ++x )
	for( int z = 0; z < CHUNK_X; ++z )
	{
		int depth = 0;
		for( int y = CHUNK_Y - 1; y >= 0; --y )
		{
			auto block = chunk.fast_get(x, y, z);
			if( block != Blocks::AIR ) ++depth;
			else depth = 0;

			if(depth == 1)
			{
				chunk.fast_set( x, y, z, Blocks::GRASS );

				if( d(e) )
					push_structure( m_tree_struct, x + chunk.m_position.x, y, z + chunk.m_position.y );
			}

			else if( depth > 1 && depth <= 4 )
				chunk.fast_set( x, y, z, Blocks::DIRT );
		}
	}
}

void TerrainGenerator::place_blocks( Chunk &chunk )
{
	auto chunk_blocks = m_chunk_blocks.find( {chunk.m_position.x, chunk.m_position.y} );
	if( chunk_blocks == m_chunk_blocks.end() ) return;

	auto &blocks = chunk_blocks->second;

	while( !blocks.empty() )
	{
		auto block = blocks.back();
		blocks.pop_back();
		chunk.fast_set( block.x, block.y, block.z, block.block );
	}
}

void TerrainGenerator::push_structure( const Structure &structure, int px, int py, int pz )
{
	for( int x = 0; x < structure.x_length; ++x )
	for( int z = 0; z < structure.z_length; ++z )
	{
		auto [chunk_x, chunk_z] = get_pos_of_chunk(px+x, pz+z);
		auto [coord_x, coord_z] = get_pos_inside_chunk(px+x, pz+z);

		auto &block_vector = m_chunk_blocks[{chunk_x*CHUNK_X, chunk_z*CHUNK_Z}];

		for( int y = 0; y < structure.y_length && (py + y < CHUNK_Y); ++y )
		{
			ToBePlaced block_info (coord_x, py + y, coord_z, structure.get(x, y, z));
			if( block_info.block != Blocks::BlockType::AIR )
				block_vector.push_back( block_info );
		}
	}
}

#include "terrain_generator.hpp"
#include "chunk.hpp"

#include <cassert>

static float map( float v, float min1, float max1, float min2, float max2)
{
	return min2 + ((v - min1) / (max1 - min1)) * (max2-min2);
}

static float to_01( float v, float m, float M )
{
	return (v - m) / (M - m);
}

TerrainGenerator::TerrainGenerator()
{
}

Chunk &TerrainGenerator::generate(Chunk& chunk)
{
	make_shape(chunk);
	paint_blocks(chunk);

	chunk.m_should_update = true;
	return chunk;
}

void TerrainGenerator::make_shape(Chunk& chunk)
{

	constexpr float s1{ 0.01f };
	constexpr float s2{ 0.05f };
	constexpr float s3{ 0.003f };

	constexpr float mini = 10;
	constexpr float maxi = 60;
	constexpr float delta_h = maxi - mini;

	for(int x = 0; x < CHUNK_X; x++)
	for(int y = 0; y < CHUNK_Y; y++)
	for(int z = 0; z < CHUNK_Z; z++)
	{
		float X = x + chunk.m_position.x;
		float Y = 0;
		float Z = z + chunk.m_position.y;

		if( y >= mini )
			Y = (y - mini);

		float mask = to_01(m_noise.noise(X * s3, Z*s3), -1, 1);
		float value = to_01(m_noise.fractal(3, X * s1, 0, Z * s1), -1, 1);
		value = value * mask - (Y / delta_h);
		value += m_noise.fractal(3, X * s2, y * s2, Z * s2 ) * 0.3f * mask;
		

		if (value < 0) continue;
		chunk.fast_set(x, y, z, Blocks::STONE);
	}
}

void TerrainGenerator::paint_blocks(Chunk& chunk)
{
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
				chunk.fast_set( x, y, z, Blocks::GRASS );

			else if( depth > 1 && depth <= 4 )
				chunk.fast_set( x, y, z, Blocks::DIRT );
		}
	}
}
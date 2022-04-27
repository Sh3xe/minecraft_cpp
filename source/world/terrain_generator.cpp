#include "terrain_generator.hpp"
#include "chunk.hpp"

/*
int X = x + chunk.m_position.x;
int Y = y;
int Z = z + chunk.m_position.y;

float value = map(m_noise.fractal(3, X* scale, 0.0f, Z * scale), -1, 1, 0, 1);
value = value - (Y / static_cast<float>(CHUNK_Y)) * (Y / static_cast<float>(CHUNK_Y));
value += m_noise.noise(X * scale, Y * scale, Z * scale) * 0.1f;

if (value < 0) continue;
chunk.fast_set(x, y, z, Blocks::DIRT);*/

constexpr float scale{ 0.002f };
constexpr float scale2{ 0.02f };

float map( float v, float min1, float max1, float min2, float max2)
{
	return min2 + ((v - min1) / (max1 - min1)) * (max2-min2);
}

TerrainGenerator::TerrainGenerator()
{
}

Chunk &TerrainGenerator::generate(Chunk& chunk)
{
	for (int x = 0; x < CHUNK_X; x++)
	for (int z = 0; z < CHUNK_Z; z++)
	{
		int X = x + chunk.m_position.x;
		int Z = z + chunk.m_position.y;

		float val = map( m_noise.fractal(8, scale*X, scale*Z), -1, 1, 0, 1);
		val *= val;

		int h = 27 + val * 100;

		for (int y = 0; y < h - 5; y++)
			chunk.fast_set(x, y, z, Blocks::STONE);

		for (int y = h - 5; y < h - 1; y++)
			chunk.fast_set(x, y, z, Blocks::DIRT);

		chunk.fast_set(x, h-1, z, Blocks::GRASS);

		for (int y = h; y < 40; ++y)
			chunk.fast_set(x, y, z, Blocks::WATER);

	}

	chunk.m_should_update = true;
	return chunk;
}
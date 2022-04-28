#pragma once

#include "../core/simplex_noise.hpp"

class Chunk;

class TerrainGenerator
{
public:
	TerrainGenerator();
	Chunk &generate(Chunk& chunk);

private:
	void make_shape(Chunk& chunk);
	void paint_blocks(Chunk& chunk);

	SimplexNoise m_noise;

};
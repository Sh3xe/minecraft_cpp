#pragma once

#include "../core/simplex_noise.hpp"

class Chunk;

class TerrainGenerator
{
public:
	TerrainGenerator();
	Chunk &generate(Chunk& chunk);

private:
	SimplexNoise m_noise;

};
#pragma once

#include <vector>
#include <array>
#include <map>
#include <utility>
#include <string>

#include "blocks.hpp"
#include "chunk.hpp"

//#include "core/perlin.hpp"
#include "core/simplex_noise.hpp"

class World;

struct ToBePlaced
{
	ToBePlaced(int x, int y, int z, std::pair< BlockID, bool > block ):
		x( static_cast<uint8_t>(x) ),
		y( static_cast<uint8_t>(y) ),
		z( static_cast<uint8_t>(z) ),
		block(block)
	{
	}
		
	uint8_t x, y, z;
	std::pair< BlockID, bool > block;
};

using ChunkToBePlace = std::map< std::pair<int, int>, std::vector<ToBePlaced>>;
using NoiseMap = std::array< std::array< float , CHUNK_SIDE>, CHUNK_SIDE >;

class TerrainGenerator
{
public:
	TerrainGenerator( BlockDB &db, World *world );
	Chunk &generate( Chunk& chunk );

private:
	void make_shape( Chunk& chunk, int x, int y );
	
	void paint_blocks( Chunk& chunk, int x, int y );

	void push_structure( const Structure &structure, int px, int py, int cz );

private:
	float m_surface_min{ 5 };
	float m_surface_max{ 60 };
	const float water_level { 10 };

	NoiseMap m_height;
	NoiseMap m_mask;

	//PerlinNoise m_noise;
	SimplexNoise m_noise;
	std::array<Structure*, 9> m_trees;
	
	BlockDB *m_db;
	World *m_world; // pour un tronçon donnée, la liste des blocks à placer
};
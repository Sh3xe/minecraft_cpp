#pragma once

#include <vector>
#include <map>
#include <utility>
#include <string>

#include "blocks.hpp"
#include "core/simplex_noise.hpp"

class World;

struct ToBePlaced
{
	ToBePlaced(int x, int y, int z, BlockID block ):
		x( static_cast<uint8_t>(x) ),
		y( static_cast<uint8_t>(y) ),
		z( static_cast<uint8_t>(z) ),
		block(block)
	{
	}
		
	uint8_t x, y, z;
	BlockID block;
};

using ChunkToBePlace = std::map< std::pair<int, int>, std::vector<ToBePlaced>>;

class Chunk;

class TerrainGenerator
{
public:
	TerrainGenerator( BlockDB &db, World *world );
	Chunk &generate( Chunk& chunk );

private:
	void make_shape( Chunk& chunk );
	void paint_blocks( Chunk& chunk );
	void push_structure( const Structure &structure, int px, int py, int cz );

private:
	float m_surface_min{ 5 };
	float m_surface_max{ 60 };
	const float water_level { 10 };

	SimplexNoise m_noise;
	BlockDB *m_db;
	std::vector<Structure> m_structs;
	World *m_world; // pour un tronçon donnée, la liste des blocks à placer
};
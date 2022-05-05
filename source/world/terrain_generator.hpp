#pragma once

#include <vector>
#include <array>
#include <map>
#include <utility>
#include <string>

#include "blocks.hpp"
#include "chunk.hpp"

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

	void load_structs();

private:
	void shape_underworld( Chunk& chunk, int x, int z );
	void shape_caves( Chunk& chunk, int x, int z );
	void shape_surface( Chunk& chunk, int x, int z );
	void shape_sky( Chunk& chunk, int x, int z );

	void paint_underworld( Chunk& chunk, int x, int z );
	void paint_caves( Chunk& chunk, int x, int z );
	void paint_surface( Chunk& chunk, int x, int z );
	void paint_sky( Chunk& chunk, int x, int z );

	void push_structure( const Structure &structure, int px, int py, int cz );

private:
	const float m_caverns_min { 40 };
	const float m_caverns_max { 105 };
	const float m_ocean_bottom { 105 };
	const float m_water_level { 117 };
	const float m_surface_min { 120 };
	const float m_surface_max { 180 };
	const float m_sky_min { 220 };

	NoiseMap m_height;
	NoiseMap m_mask;
	NoiseMap m_ocean;

	SimplexNoise m_noise;
	std::array<const Structure*, 9> m_trees;
	std::array<BlockID, 5> m_flowers;
	
	BlockDB *m_db;
	World *m_world; // pour un tronçon donnée, la liste des blocks à placer
};
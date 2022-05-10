#pragma once

#include <vector>
#include <array>
#include <map>
#include <utility>
#include <string>
#include <random>

#include "blocks.hpp"
#include "chunk.hpp"

#include "math/simplex_noise.hpp"

class World;

struct ToBePlaced
{
	ToBePlaced(int x, int y, int z, std::pair< BlockType, bool > block ):
		x( static_cast<uint8_t>(x) ),
		y( static_cast<uint8_t>(y) ),
		z( static_cast<uint8_t>(z) ),
		block(block)
	{
	}
		
	uint8_t x, y, z;
	std::pair< BlockType, bool > block;
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
	void add_ore( Chunk &chunk, int x, int y, int z, BlockType ore );

	BlockType get_ore_type( int y );

private:
	const float m_caverns_min { 25 };
	const float m_caverns_max { 70 };
	const float m_ocean_bottom { 70 };
	const float m_water_level { 80 };
	const float m_surface_min { 85 };
	const float m_surface_max { 150 };
	const float m_sky_min { 150 };
	const float m_sky_max { 185 };

	NoiseMap m_height;
	NoiseMap m_mask;
	NoiseMap m_ocean;

	SimplexNoise m_noise;
	std::default_random_engine m_rd;

	std::array<const Structure*, 9> m_trees;
	std::array<BlockType, 6> m_flowers;
	
	BlockDB *m_db;
	World *m_world; // pour un tronçon donnée, la liste des blocks à placer
};
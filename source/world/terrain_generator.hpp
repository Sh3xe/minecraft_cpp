#pragma once

#include <vector>
#include <map>
#include <utility>

#include "world/structure.hpp"
#include "core/simplex_noise.hpp"

class Chunk;

class TerrainGenerator
{
public:
	TerrainGenerator();
	Chunk &generate( Chunk& chunk );

private:
	struct ToBePlaced
	{
		ToBePlaced(int x, int y, int z, Blocks::BlockType block ):
			x( static_cast<uint8_t>(x) ),
			y( static_cast<uint8_t>(y) ),
			z( static_cast<uint8_t>(z) ),
			block(block)
		{
		}
		 
		uint8_t x, y, z;
		Blocks::BlockType block;
	};

	void make_shape( Chunk& chunk );
	void paint_blocks( Chunk& chunk );
	void push_structure( const Structure &structure, int px, int py, int cz );
	void place_blocks( Chunk &chunk );

private:
	float m_surface_min{ 10 };
	float m_surface_max{ 60 };
	SimplexNoise m_noise;
	Structure m_tree_struct;
	std::map< std::pair<int, int>, std::vector<ToBePlaced>> m_chunk_blocks; // pour un tronçon donnée, la liste des blocks à placer
};
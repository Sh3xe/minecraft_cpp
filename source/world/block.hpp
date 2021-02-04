#ifndef BLOCK_INCLUDED_
#define BLOCK_INCLUDED_

namespace Blocks {

	enum BlockType: unsigned char {
		AIR = 0,
		DIRT,
		GRASS,
		WOOD,
		STONE,
		MOSSY_STONE,
		IRON_ORE,
		COAL,
		DIAMOND_ORE,
		GOLD_ORE,
		COBBLESTONE,
		MOSSY_COBBLESTONE,
		LEAF,
		SAND,
		MOSSY_BRICK,
		BRICK,
		SNOW_GRASS
	};

	// +x, -x, +y, -y, +z, -z
	const unsigned char block_faces[][6] {
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{1, 1, 2, 0, 1, 1},
		{4, 4, 3, 3, 4, 4},
		{5, 5, 5, 5, 5, 5},
		{6, 6, 6, 6, 6, 6},
		{7, 7, 7, 7, 7, 7},
		{8, 8, 8, 8, 8, 8},
		{9, 9, 9, 9, 9, 9},
		{10, 10, 10, 10, 10, 10},
		{11, 11, 11, 11, 11, 11},
		{12, 12, 12, 12, 12, 12},
		{13, 13, 13, 13, 13, 13},
		{14, 14, 14, 14, 14, 14},
		{15, 15, 15, 15, 15, 15},
		{16, 16, 16, 16, 16, 16},
		{17, 17, 18, 0, 17, 17}
	};

	const BlockType tree_blocks[36] {
		AIR, AIR, AIR, AIR, WOOD, AIR, AIR, AIR, AIR,
		LEAF, LEAF, LEAF, LEAF, WOOD, LEAF, LEAF, LEAF, LEAF,
		AIR, LEAF, AIR, LEAF, WOOD, LEAF, AIR, LEAF, AIR,
		AIR, AIR, AIR, AIR, LEAF, AIR, AIR, AIR, AIR,
	};
}

#endif
#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <array>

namespace blk
{

// enums for block types and structure types
enum class BlockType: uint8_t
{
	air = 0,
	dirt,
	grass,
	log,
	stone,
	mossy_stone,
	iron_ore,
	coal_ore,
	diamond_ore,
	gold_ore,
	cobblestone,
	mossy_cobblestone,
	leaf,
	sand,
	mossy_brick,
	brick,
	snow_grass,
	water,
	wood_plank,
	gravel,

	bluesmallflower,
	blueflower,
	purplesmallflower,
	purpleflower,
	xgrass,
	smalltree
};

enum class StructType: uint8_t
{
	bigtree1 = 0,
	bigtree2,
	bigtree3,
	mediumtree1,
	mediumtree2,
	mediumtree3,
	smalltree1,
	smalltree2,
	smalltree3,
	rock1,
	rock2
};

// shape of the mesh of the block
enum class BlockShape: uint8_t
{
	cubic,
	x
};

// holds data of a minecraft texture (blocks, size, center)
struct StructData
{
	std::string name;
    uint16_t x_length {0};
    uint16_t y_length {0};
    uint16_t z_length {0};
	uint16_t center_x {0};
    uint16_t center_y {0};
    uint16_t center_z {0};
    std::vector< std::pair<BlockType, bool> > blocks;
    
    inline const std::pair<BlockType, bool> &get( int x, int y, int z ) const { return blocks[y*x_length*z_length+z*x_length+x]; }
};

// same for blocks
struct BlockData
{
	std::string name      {};
	std::string smallname {};

	bool visible    {false};
	bool collidable {false};

	BlockShape shape             { BlockShape::cubic };
	std::array<uint8_t, 6> faces {};
	uint8_t mesh_group           {0};
};

// loads structures data from disk
bool load_structures( const std::string &path );

// get the data from a block type
BlockData get_block( BlockType id );
const StructData &get_struct( StructType id );

} // namespace blk
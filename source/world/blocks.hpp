#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <array>

using BlockID = uint8_t;

enum class BlockShape: uint8_t
{
	cubic,
	x
};

struct Structure
{
    uint16_t x_length {0};
    uint32_t y_length {0};
    uint32_t z_length {0};
    std::vector<BlockID> blocks;
    
    inline const BlockID get( int x, int y, int z ) const { return blocks[y*x_length*z_length+z*x_length+x]; }
};

struct BlockType
{
	std::string name;
	std::string smallname;
	BlockID id;

	bool visible;
	bool collidable;

	std::array<uint8_t, 6> faces;
	BlockShape shape;
	uint8_t mesh_group;
};

class BlockDB
{
public:
	BlockDB()
	{	
	}
	
	BlockDB operator=( const BlockDB& ) = delete;

	bool load_blocks_from_file( const std::string &path );
	bool load_structures_from_file( const std::string &path );

	const BlockType &get_block( BlockID id ) const;
	const BlockID &id_from_name( const std::string &name );
	const BlockID &id_from_smallname( const std::string &smallname );

	const Structure &get_struct( const std::string &name );

private:
	std::map< std::string, Structure > m_structures;

	std::vector<BlockType> m_blocks;
	std::map<std::string, BlockID> m_names;
	std::map<std::string, BlockID> m_smallnames;

};
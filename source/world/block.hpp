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
	bool load_from_file( const std::string &path );

	const BlockType &id_get( BlockID id ) const;
	const BlockType &name_get( const std::string &name );
	const BlockType &smallname_get( const std::string &smallname );

private:
	std::vector<BlockType> m_blocks;
	std::map<std::string, BlockID> m_names;
	std::map<std::string, BlockID> m_smallnames;

};
#pragma once

#include <cstdint>
#include <string>
#include <vector>

using BlockID = uint8_t;

enum class BlockShape: uint8_t
{
	cubic,
	x
};

struct BlockFaces
{
	// id into texture atlas
	uint8_t up, down, left, right, front, back;
};

struct BlockType
{
	std::string name;
	std::string smallname;
	BlockID id;

	bool visible;
	bool collidable;

	BlockShape shape;
	uint8_t mesh_group;
};

class BlockDB
{
public:
	BlockDB operator=( const BlockDB& ) = delete;
	BlockDB( const BlockDB& ) = delete;

	inline static BlockDB &get()
	{
		static BlockDB instance;
		return instance;
	}

	void load_from_file( const std::string &path );

	const BlockType &id_get( BlockID id ) const;
	const BlockType &name_get( const std::string &name ) const;
	const BlockType &smallname_get( const std::string &smallname ) const;

private:
	BlockDB();

	std::vector<BlockType> m_blocks;

};
#include "blocks.hpp"
#include <cassert>
#include <fstream>
#include <filesystem>
#include <optional>
#include <map>
#include "core/logger.hpp"

namespace fs = std::filesystem;

namespace blk
{

// D A T A
static std::array< BlockData, 256 > block_data =
{
	BlockData { "air", "a", false, false, BlockShape::cubic, std::array<uint8_t, 6>{0, 0, 0, 0, 0, 0}, MeshGroup::block},
	BlockData { "dirt", "d", true, true, BlockShape::cubic, std::array<uint8_t, 6>{0, 0, 0, 0, 0, 0}, MeshGroup::block},
	BlockData { "grass", "g", true, true, BlockShape::cubic, std::array<uint8_t, 6>{1, 1, 2, 0, 1, 1}, MeshGroup::block},
	BlockData { "log", "l", true, true, BlockShape::cubic, std::array<uint8_t, 6>{4, 4, 3, 3, 4, 4}, MeshGroup::block},
	BlockData { "stone", "s", true, true, BlockShape::cubic, std::array<uint8_t, 6>{5, 5, 5, 5, 5, 5}, MeshGroup::block},
	BlockData { "mossy_stone", "ms", true, true, BlockShape::cubic, std::array<uint8_t, 6>{6, 6, 6, 6, 6, 6}, MeshGroup::block},
	BlockData { "iron_ore", "io", true, true, BlockShape::cubic, std::array<uint8_t, 6>{7, 7, 7, 7, 7, 7}, MeshGroup::block},
	BlockData { "coal_ore", "co", true, true, BlockShape::cubic, std::array<uint8_t, 6>{8, 8, 8, 8, 8, 8}, MeshGroup::block},
	BlockData { "diamond_ore", "do", true, true, BlockShape::cubic, std::array<uint8_t, 6>{9, 9, 9, 9, 9, 9}, MeshGroup::block},
	BlockData { "gold_ore", "go", true, true, BlockShape::cubic, std::array<uint8_t, 6>{10, 10, 10, 10, 10, 11}, MeshGroup::block},
	BlockData { "cobblestone", "c", true, true, BlockShape::cubic, std::array<uint8_t, 6>{11, 11, 11, 11, 11, 11}, MeshGroup::block},
	BlockData { "mossy_cobblestone", "mc", true, true, BlockShape::cubic, std::array<uint8_t, 6>{12, 12, 12, 12, 12, 12}, MeshGroup::block},
	BlockData { "leaf", "le", true, true, BlockShape::cubic, std::array<uint8_t, 6>{13, 13, 13, 13, 13, 13}, MeshGroup::foliage},
	BlockData { "sand", "sd", true, true, BlockShape::cubic, std::array<uint8_t, 6>{14, 14, 14, 14, 14, 14}, MeshGroup::block},
	BlockData { "mossy_brick", "mb", true, true, BlockShape::cubic, std::array<uint8_t, 6>{15, 15, 15, 15, 15, 15}, MeshGroup::block},
	BlockData { "brick", "b", true, true, BlockShape::cubic, std::array<uint8_t, 6>{16, 16, 16, 16, 16, 16}, MeshGroup::block},
	BlockData { "snow_grass", "sg", true, true, BlockShape::cubic, std::array<uint8_t, 6>{17, 17, 18, 0, 17, 17}, MeshGroup::block},
	BlockData { "water", "w", true, true, BlockShape::cubic, std::array<uint8_t, 6>{17, 17, 17, 17, 17, 17}, MeshGroup::water},
	BlockData { "wood_plank", "wp", true, true, BlockShape::cubic, std::array<uint8_t, 6>{18, 18, 18, 18, 18, 18}, MeshGroup::block},
	BlockData { "gravel", "gr", true, true, BlockShape::cubic, std::array<uint8_t, 6>{19, 19, 19, 19, 19, 19}, MeshGroup::block},
	BlockData { "bluesmallflower", "bsf", true, false, BlockShape::x, std::array<uint8_t, 6>{128, 128, 128, 128, 128, 128}, MeshGroup::foliage},
	BlockData { "blueflower", "bf", true, false, BlockShape::x, std::array<uint8_t, 6>{129, 129, 129, 129, 129, 129},MeshGroup::foliage},
	BlockData { "purplesmallflower", "psf", true, false, BlockShape::x, std::array<uint8_t, 6>{130, 130, 130, 130, 130, 130}, MeshGroup::foliage},
	BlockData { "purpleflower", "pf", true, false, BlockShape::x, std::array<uint8_t, 6>{131, 131, 131, 131, 131, 131}, MeshGroup::foliage},
	BlockData { "xgrass", "xg", true, false, BlockShape::x, std::array<uint8_t, 6>{132, 132, 132, 132, 132, 132}, MeshGroup::foliage},
	BlockData { "treeseed", "ts", true, false, BlockShape::x, std::array<uint8_t, 6>{133, 133, 133, 133, 133, 133}, MeshGroup::foliage}
};

static bool structures_loaded = false;
static std::array< StructData, 16 > struct_data;

// functions 
BlockData get_block( BlockType id )
{
	return block_data[ static_cast<uint8_t>(id) ];
}

const StructData &get_struct( StructType id )
{
	assert( structures_loaded );
	return struct_data[ static_cast<uint8_t>(id) ];
}

static BlockType id_from_smallname( const std::string &name )
{
	for( int i = 0; i < 256; ++i )
	{
		if (block_data[i].smallname == name)
			return static_cast<BlockType>(i);
	}

	return blk::BlockType::air;
}

static std::optional<StructData> load_struct( const fs::path &file_path )
{
	std::ifstream file { file_path };

	if( !file ) return {};

	StructData data;

	file >> data.name;
	file >> data.center_x >> data.center_y >> data.center_z;
	file >> data.x_length >> data.y_length >> data.z_length;

	int volume = data.x_length * data.y_length * data.z_length;

	std::string smallname {};
	bool has_priority {false};
	for( int i = 0; i < volume; ++i )
	{
		file >> smallname >> has_priority;
		data.blocks.push_back( 
			std::make_pair( id_from_smallname( smallname ), has_priority )
		);
	}

	if( file.fail() ) return {};
	file.close();

	structures_loaded = true;

	return data;
}


bool load_structures( const std::string &file_path )
{
	std::ifstream file { fs::current_path() /= fs::path{file_path} };
	if( !file ) return false;

	std::string line;
	int current_id = 0;
	while( std::getline(file, line) )
	{
		auto data = load_struct( fs::current_path() /= fs::path{file_path}.parent_path() /= fs::path{line} );
		if( data )
			struct_data[current_id++] = data.value();
		else
		{
			SD_DEBUG( "Impossible de charger la structure: ", line );
			return false;
		}
	}

	file.close();
	return true;
}

} // namespace blk
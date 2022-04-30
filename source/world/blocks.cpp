#include "blocks.hpp"

#include "core/logger.hpp"
#include "cmake_defines.hpp"

#include <json.hpp>
#include <cassert>
#include <fstream>

using namespace nlohmann;

bool BlockDB::load_structures_from_file( const std::string &path )
{
	m_structures.clear();
    std::fstream file { std::string{ROOT_DIR} + "/" + path };

	if( !file )
	{
		SD_ERROR( "Impossible de charger: ", path );
		return false;
	}

	try
	{
		json data;
		file >> data;

		Structure structure;
		std::string name;
		for( auto &struct_data: data )
		{
			name = struct_data["name"];

			structure.x_length = struct_data["x_length"];
			structure.y_length = struct_data["y_length"];
			structure.z_length = struct_data["z_length"];

			uint32_t struct_volume = structure.x_length * structure.y_length * structure.z_length;

			structure.blocks.reserve( struct_volume );

			auto block_array = struct_data["data"];

			for( int i = 0; i < struct_volume; ++i )
				structure.blocks.push_back( id_from_smallname( block_array[i] ) );

			m_structures[name] = structure;
		}

		return true;
	}
	catch( std::runtime_error &err )
	{
		return false;
	}
}

bool BlockDB::load_blocks_from_file( const std::string &path )
{
	m_blocks.clear();
    std::fstream file { std::string{ROOT_DIR} + "/" + path };

	if( !file )
	{
		SD_ERROR( "Impossible de charger: ", path );
		return false;
	}

	try
	{
		json data;
		file >> data;

		int id = 0;
		for( auto &block_data: data )
		{
			BlockType block;

			block.id = id++;
			block.name = block_data["name"];
			block.smallname = block_data["smallname"];

			block.collidable = block_data["collidable"];
			block.visible = block_data["visible"];

			block.mesh_group = block_data["mesh_group"];

			for( int i = 0; i < 6; ++i )
				block.faces[i] = block_data["faces"][i];

			std::string shape = block_data["name"];
			if( shape == "x" )
				block.shape = BlockShape::x;
			else
				block.shape = BlockShape::cubic;

			m_blocks.push_back( block );
			m_names[ block.name ] = block.id;
			m_smallnames[ block.smallname ] = block.id;
		}

		return true;
	}
	catch( std::runtime_error &err )
	{
		return false;
	}
}

const Structure &BlockDB::get_struct( const std::string &name )
{
	return m_structures.at( name );
}

const BlockType &BlockDB::get_block( BlockID id ) const
{
	assert( id < m_blocks.size() );
	return m_blocks[id];
}

const BlockID &BlockDB::id_from_name( const std::string &name )
{
	return m_names.at(name);
}

const BlockID &BlockDB::id_from_smallname( const std::string &smallname )
{
	return m_smallnames.at(smallname);
}
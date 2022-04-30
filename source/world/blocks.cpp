#include "block.hpp"

#include "core/logger.hpp"
#include "cmake_defines.hpp"

#include <json.hpp>
#include <cassert>
#include <fstream>

using namespace nlohmann;

bool BlockDB::load_from_file( const std::string &path )
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

const BlockType &BlockDB::id_get( BlockID id ) const
{
	assert( id < m_blocks.size() );
	return m_blocks[id];
}

const BlockType &BlockDB::name_get( const std::string &name )
{
	return id_get(m_names[name]);
}

const BlockType &BlockDB::smallname_get( const std::string &smallname )
{
	return id_get(m_smallnames[smallname]);
}
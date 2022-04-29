#include "block.hpp"

#include "core/logger.hpp"
#include "cmake_defines.hpp"
#include <json.hpp>
#include <fstream>

using namespace nlohmann;

bool BlockDB::load_from_file( const std::string &path )
{
    std::fstream file { std::string{ROOT_DIR} + "/" + path };

	if( !file )
	{
		SD_ERROR( "Impossible de charger: ", path );
		return false;
	}

	json data;
	file >> data;

    for( auto &block: data )
    {

    }

    return true;
}

const BlockType &BlockDB::id_get( BlockID id ) const
{

}

const BlockType &BlockDB::name_get( const std::string &name ) const
{

}

const BlockType &BlockDB::smallname_get( const std::string &smallname ) const
{

}

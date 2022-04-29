#include "structure.hpp"
#include "core/logger.hpp"
#include "cmake_defines.hpp"
#include <fstream>

bool Structure::load_from_file( const std::string &path )
{
    std::fstream file { std::string(ROOT_DIR) + "/" + path };

    if( !file )
    {
        SD_DEBUG("Impossible d'ouvrir", path);
        return false;
    }

    file >> x_length;
    file >> z_length;
    file >> y_length;

    if( file.fail() || x_length < 0 || y_length < 0 || z_length < 0 )
    {
        SD_DEBUG("Taille de la tructure invalide");
        return false;
    }

    int b = 0;
    for( int i = 0 ; i < y_length * z_length * x_length; ++i )
    {
        file >> b;
        blocks.push_back( static_cast<Blocks::BlockType>(b) );
    }

    if( file.fail() )
    {
        SD_DEBUG("Impossible de récuperer les données de la structure");
        return false;
    }
    
    file.close();

    return true;
}
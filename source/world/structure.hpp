#pragma once

#include "block.hpp"
#include <vector>
#include <cstdint>
#include <string>

struct Structure
{
    uint16_t x_length {0};
    uint32_t y_length {0};
    uint32_t z_length {0};
    std::vector<Blocks::BlockType> blocks;

    bool load_from_file( const std::string &path );
    inline const Blocks::BlockType get( int x, int y, int z ) const { return blocks[y*x_length*z_length+z*x_length+x]; }
};
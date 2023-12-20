#pragma once

#include <cstdint>

// hold information as unsigned char ( GL_UNSIGNED_BYTE )
struct BlockVertex
{
    BlockVertex() = default;

    BlockVertex(int x, int y, int z, int tx, int ty, int num)
    {
        set_pos( static_cast<uint8_t>(x), static_cast<uint8_t>(y), static_cast<uint8_t>(z));
        set_text_coords(static_cast<uint8_t>(tx), static_cast<uint8_t>(ty));
        set_num(static_cast<uint8_t>(num));
    }

    BlockVertex(uint8_t x, uint8_t y, uint8_t z, uint8_t tx, uint8_t ty, uint8_t num )
    {
        set_pos(x, y, z);
        set_text_coords(tx, ty);
        set_num(num);
    }

    void set_pos(uint8_t x, uint8_t y, uint8_t z)
    {
        data &= 0b00000000000000000001111111111111;
        data |= (static_cast<uint32_t>(x) & 0b00011111) << 27;
        data |= (static_cast<uint32_t>(y) & 0b111111111) << 13;
        data |= (static_cast<uint32_t>(z) & 0b00011111) << 22;
    }

    void set_text_coords(uint8_t tx, uint8_t ty)
    {
        data &= 0b11111111111111111110000000000111;
        data |= (static_cast<uint32_t>(tx) & 0b00011111) << 8;
        data |= (static_cast<uint32_t>(ty) & 0b00011111) << 3;
    }

    void set_num(uint8_t num)
    {
        data &= 0b11111111111111111111111111111000;
        data |= (static_cast<uint32_t>(num) & 0b0111);
    }

    uint32_t data;
};
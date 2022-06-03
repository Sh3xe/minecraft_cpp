#pragma once

#include <cstdint>

struct Vertex4Byte
{
    Vertex4Byte() = default;

    Vertex4Byte(int x, int y, int z, int tx, int ty, int normal)
    {
        set_pos( static_cast<uint8_t>(x), static_cast<uint8_t>(y), static_cast<uint8_t>(z));
        set_text_coords(static_cast<uint8_t>(tx), static_cast<uint8_t>(ty));
        set_normal(static_cast<uint8_t>(normal));
    }

    Vertex4Byte(uint8_t x, uint8_t y, uint8_t z, uint8_t tx, uint8_t ty, uint8_t normal )
    {
        set_pos(x, y, z);
        set_text_coords(tx, ty);
        set_normal(normal);
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

    void set_normal(uint8_t normal)
    {
        data &= 0b11111111111111111111111111111000;
        data |= (static_cast<uint32_t>(normal) & 0b0111);
    }

    uint32_t data;
};

struct Vertex8Byte
{
    Vertex8Byte() = default;

    Vertex8Byte(int x, int y, int z, int tx, int ty, int normal)
    {
        set_pos( static_cast<uint8_t>(x), static_cast<uint8_t>(y), static_cast<uint8_t>(z));
        set_text_coords(static_cast<uint8_t>(tx), static_cast<uint8_t>(ty));
        set_normal(static_cast<uint8_t>(normal));
    }

    Vertex8Byte(uint8_t x, uint8_t y, uint8_t z, uint8_t tx, uint8_t ty, uint8_t normal )
    {
        set_pos(x, y, z);
        set_text_coords(tx, ty);
        set_normal(normal);
    }

    void set_pos(uint8_t x, uint8_t y, uint8_t z)
    {
        data &= 0xffff00000000ffff;
        data |= (static_cast<uint64_t>(x) & 0x00ff) << 24;
        data |= (static_cast<uint64_t>(z) & 0x00ff) << 32;
        data |= (static_cast<uint64_t>(y) & 0xffff) << 40;
    }

    void set_text_coords(uint8_t tx, uint8_t ty)
    {
        data &= 0xffffffffffff00ff;
        data |= (static_cast<uint64_t>(tx) & 0x0f) << 8;
        data |= (static_cast<uint64_t>(ty) & 0x0f) << 16;
    }

    void set_normal(uint8_t normal)
    {
        data &= 0xffffffffffffff00;
        data |= (static_cast<uint64_t>(normal) & 0xff);
    }

    uint64_t data;
};
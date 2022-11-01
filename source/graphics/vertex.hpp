#pragma once

#include <cstdint>
#include <cassert>

/*
    x, z  : sur 5 bits (0-31)
    y     : sur 9 bits (0-511)
    tx, ty: sur 5 bits (0-31)
    normal: sur 3 bits (0-7)
*/
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

/*
    x, z  : sur 8 bits (0-255)   rogné sur [0-128]
    y     : sur 12 bits (0-4095) rogné sur [0-2048]
    tx, ty: sur 9 bits (0-511)   rogné sur [0-256]
    normal: sur 8 bits (0-31)    non rogné

    x: 1; y: 2; z: 3; tx: 4; ty: 5; norm: 6    (0: rien)
    00000000 00666666 66555555 55544444  44442222 22222222 33333333 11111111
*/
struct Vertex8Byte
{
    Vertex8Byte() = default;

    Vertex8Byte(int x, int y, int z, int tx, int ty, int normal)
    {
        set_pos( static_cast<uint8_t>(x), static_cast<uint16_t>(y), static_cast<uint8_t>(z));
        set_text_coords(static_cast<uint16_t>(tx), static_cast<uint16_t>(ty));
        set_normal(static_cast<uint8_t>(normal));
    }

    Vertex8Byte(uint8_t x, uint16_t y, uint8_t z, uint16_t tx, uint16_t ty, uint8_t normal )
    {
        set_pos(x, y, z);
        set_text_coords(tx, ty);
        set_normal(normal);
    }

    void set_pos(uint8_t x, uint16_t y, uint8_t z)
    {
        assert( x <= 128 && z <= 128 && y <= 2048);
        data &= 0xfffffffff0000000;
        data |= (static_cast<uint64_t>(x) & 0x00ff) << 0;
        data |= (static_cast<uint64_t>(z) & 0x00ff) << 8;
        data |= (static_cast<uint64_t>(y) & 0x0fff) << 16;
    }

    void set_text_coords(uint16_t tx, uint16_t ty)
    {
        assert(tx <= 256 && ty <= 256);
        data &= 0b1111111111111111110000000000000000001111111111111111111111111111;

        data |= (static_cast<uint64_t>(tx) & 0b111111111) << 28;
        data |= (static_cast<uint64_t>(ty) & 0b111111111) << 37;
    }

    void set_normal(uint8_t normal)
    {
        data &= 0b1111111111000000001111111111111111111111111111111111111111111111;
        data |= (static_cast<uint64_t>(normal) & 0xff) << 46;
    }

    uint64_t data;
};

template <typename T>
constexpr bool is_vertex_type() { return false; }

template<>
constexpr bool is_vertex_type<Vertex4Byte>() { return true; }

template<>
constexpr bool is_vertex_type<Vertex8Byte>() { return true; }

/*

    00000000 00000000 00000000 00000000  00000000 00000000 00000000 00000000
    11111111 11000000 00111111 11111111  11111111 11111111 11111111 11111111*/
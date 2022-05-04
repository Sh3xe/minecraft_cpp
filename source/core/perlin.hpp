#pragma once

#include <array>

// implementaion based on https://cs.nyu.edu/~perlin/noise/

class PerlinNoise
{
public:
	PerlinNoise();
	PerlinNoise( int seed );

	float noise( float x, float y, float z, float factor = 1.0f ) const;

private:
	float lerp( float t, float a, float b ) const;
	float fade( float t ) const;
	float grad( int hash, float x, float y, float z ) const;

	std::array<unsigned char, 512> m_table;

};
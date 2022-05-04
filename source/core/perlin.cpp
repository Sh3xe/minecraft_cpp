#include "perlin.hpp"

#include <iostream>

#include <random>
#include <algorithm>
#include <cmath>

// implementaion based on https://cs.nyu.edu/~perlin/noise/

PerlinNoise::PerlinNoise():
	PerlinNoise(0) {
}

PerlinNoise::PerlinNoise(int seed) {
	// fill permutation table
	for(int i = 0; i < 256; ++i)
		m_table[i] = i;
	

	// suffle permutation table
	std::default_random_engine random_engine(seed);
	std::shuffle(m_table.begin(), m_table.begin()+256, random_engine);

	for(int i = 0; i < 256; ++i)
		m_table[i+256] = m_table[i];
}

float PerlinNoise::grad(int hash, float x, float y, float z) const {
	int h = hash & 15;
	float u = (h<8) ? x : y,
		   v = (h<4) ? y : (h==12||h==14) ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float PerlinNoise::lerp(float t, float a, float b) const {
	return a + ( b - a) * t;
}

float PerlinNoise::fade(float t) const {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::noise(float x, float y, float z, float factor) const{

	x *= factor;
	y *= factor;
	z *= factor;

	int X = static_cast<int>(floor(x)) & 255,
		Y = static_cast<int>(floor(y)) & 255,
		Z = static_cast<int>(floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	float u = fade(x),
		   v = fade(y),
		   w = fade(z);

	int A = m_table[X  ]+Y, AA = m_table[A]+Z, AB = m_table[A+1]+Z,
		B = m_table[X+1]+Y, BA = m_table[B]+Z, BB = m_table[B+1]+Z;

	return lerp(w, lerp(v, lerp(u, grad(m_table[AA], x  , y  , z   ),
									grad(m_table[BA], x-1, y  , z   )),
							lerp(u, grad(m_table[AB], x  , y-1, z   ),
									grad(m_table[BB], x-1, y-1, z   ))),
					lerp(v, lerp(u, grad(m_table[AA+1], x  , y  , z-1 ),
									grad(m_table[BA+1], x-1, y  , z-1 )),
							lerp(u, grad(m_table[AB+1], x  , y-1, z-1 ),
									grad(m_table[BB+1], x-1, y-1, z-1 ))));
}
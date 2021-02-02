#include "perlin.hpp"

#include <random>
#include <iostream>
#include <algorithm>

using vec2d = std::pair<double, double>;

double dot( const vec2d &p1, const vec2d &p2) {
	return p1.first * p2.first + p1.second * p2.second;
}

PerlinNoise::PerlinNoise():
	PerlinNoise(0) {
}

PerlinNoise::PerlinNoise(int seed) {
	// fill permutation table
	for(int i = 0; i < 256; ++i)
		m_table[i] = i;

	// suffle permutation table
	std::default_random_engine random_engine(seed);
	std::shuffle(m_table.begin(), m_table.end(), random_engine);

	// fill m_vectors array
	double sqrt2 = 1.41421356237;
	m_vectors = {
		vec2d(1.0   , 0.0   ),
		vec2d(-1.0   , 0.0   ),
		vec2d(0.0   , 1.0   ),
		vec2d(0.0   , -1.0   ),
		vec2d(-sqrt2,  sqrt2),
		vec2d(-sqrt2, -sqrt2),
		vec2d( sqrt2, -sqrt2),
		vec2d( sqrt2,  sqrt2)
	};
}

const vec2d &PerlinNoise::getConstantVector(int index) {
	return m_vectors[index % 8];
}

double PerlinNoise::lerp(double a, double b, double t) {
	return a + ( b - a) * t;
}

double PerlinNoise::fade(double t) {
	return ((6*t - 15)*t + 10)*t*t*t;
}

double PerlinNoise::noise(double x, double y) {

	if (x < 0) x = -x;
	if (y < 0) y = -y;

	int gx0 = static_cast<int>(x) % 15,
		gy0 = static_cast<int>(y) % 15,
		gx1 = (gx0 + 1) % 15,
		gy1 = (gy0 + 1) % 15;

	double dtl = dot( getConstantVector(m_table[gx0 + gy0 * 16]), vec2d(x - gx0, y - gy0) ),
		   dtr = dot( getConstantVector(m_table[gx1 + gy0 * 16]), vec2d(x - gx1, y - gy0) ),
		   dbl = dot( getConstantVector(m_table[gx0 + gy1 * 16]), vec2d(x - gx0, y - gy1) ),
		   dbr = dot( getConstantVector(m_table[gx1 + gy1 * 16]), vec2d(x - gx1, y - gy1) );

	return lerp(
		lerp(dtl, dbl, fade(y - gy0)),
		lerp(dtr, dbr, fade(y - gy0)),
		fade(x - gx0)
	);
}
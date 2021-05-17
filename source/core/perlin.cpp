#include "perlin.hpp"

#include <random>
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
		vec2d(-1.0  , 0.0   ),
		vec2d(0.0   , 1.0   ),
		vec2d(0.0   , -1.0  ),
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

double PerlinNoise::noise(double x, double y, double factor) {

	x *= factor;
	y *= factor;

	while (x < 0) x += 16.0; // temporary: will be replaced by a combination of multiplication,
	while (y < 0) y += 16.0; // division and floor (when other bugs are handled)


	// get the grid x, y, as well as x + 1 and y + 1
	int gx0 = static_cast<int>(x) % 16, // + 16,
		gy0 = static_cast<int>(y) % 16;

	//if (gx0 < 0) gx0 += 16;
	//if (gy0 < 0) gy0 += 16;

	int gx1 = (gx0 + 1) % 16,
		gy1 = (gy0 + 1) % 16;

	// calculate the vector between x, y and grid x, grid y (same for grid x + 1, grid_y + 1
	double vx0 = x - static_cast<int>(x),
		   vy0 = y - static_cast<int>(y),
		   vx1 = x - static_cast<int>(x) - 1,
		   vy1 = y - static_cast<int>(y) - 1;

	// get dot products
	double dtl = dot( getConstantVector(m_table[gx0 + gy0 * 16]), vec2d( vx0,  vy0) ),
		   dtr = dot( getConstantVector(m_table[gx1 + gy0 * 16]), vec2d(-vx1,  vy0) ),
		   dbl = dot( getConstantVector(m_table[gx0 + gy1 * 16]), vec2d( vx0, -vy1) ),
		   dbr = dot( getConstantVector(m_table[gx1 + gy1 * 16]), vec2d(-vx1, -vy1) );

	// interpolate
	return lerp(
		lerp(dtl, dbl, fade(vy0)),
		lerp(dtr, dbr, fade(vy0)),
		fade(vx0)
	);
}
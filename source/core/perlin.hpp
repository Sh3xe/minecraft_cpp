#ifndef PERLIN_HPP_
#define PERLIN_HPP_

#include <array>

// implementaion based on https://cs.nyu.edu/~perlin/noise/

class PerlinNoise {
public:
	PerlinNoise();
	PerlinNoise(int seed);

	double noise(double x, double y, double z, double factor = 1.0);

private:
	double lerp(double t, double a, double b);
	double fade(double t);
	double grad(int hash, double x, double y, double z);

	std::array<unsigned char, 512> m_table;

};

#endif
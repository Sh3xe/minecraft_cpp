#ifndef PERLIN_HPP_
#define PERLIN_HPP_

#include <array>

class PerlinNoise {
public:
	PerlinNoise();
	PerlinNoise(int seed);

	double noise(double x, double y);

private:
	double lerp(double a, double b, double t);
	double fade(double t);
	const std::pair<double, double> &getConstantVector(int index);

	std::array<unsigned char, 256> m_table;
	std::array< std::pair<double, double>, 8> m_vectors;

};

#endif
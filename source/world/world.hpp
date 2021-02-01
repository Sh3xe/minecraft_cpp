#ifndef WORLD_INCLUDED_
#define WORLD_INCLUDED_

#include <map>
#include <memory>
#include "chunk.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"
#include "../core/perlin.hpp"

#define WORLD_X 8
#define WORLD_Z 8

class Camera;

class World {
public:
	World();
	~World();

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);

	void draw( Camera &camera );

private:
	Shader m_shader;
	Texture m_tilset;
	PerlinNoise m_noise_generator;

	std::map< std::pair<int, int>, Chunk*> m_chunks;
	//std::map< std::pair<int, int> , std::unique_ptr<Chunk>> m_chunks;
	//Chunk *m_chunks;
};


#endif
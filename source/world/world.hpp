#ifndef WORLD_INCLUDED_
#define WORLD_INCLUDED_

#include <map>
#include <memory>
#include "chunk.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"
#include "../core/perlin.hpp"

#define WORLD_X 7
#define WORLD_Z 7

class Camera;

class World {
public:
	World();
	~World();

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);

	void draw( Camera &camera );
	void update( double delta_time, Camera &camera );

private:
	void updateChunksNeighbours();

	Shader m_shader;
	Texture m_tilset;
	PerlinNoise m_noise_generator;

	std::map< std::pair<int, int>, std::unique_ptr<Chunk>> m_chunks;
	//std::map< std::pair<int, int>, Chunk*> m_chunks;
};


#endif
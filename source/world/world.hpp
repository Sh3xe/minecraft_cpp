#ifndef WORLD_INCLUDED_
#define WORLD_INCLUDED_

#include "chunk.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"

class Camera;

class World {
public:
	World();

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);

	void draw( Camera &camera );

private:
	Shader m_shader;
	Texture m_tilset;

	Chunk m_chunks[4][4];

};


#endif
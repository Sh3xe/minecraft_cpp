#ifndef WORLD_INCLUDED_
#define WORLD_INCLUDED_

#include "chunk.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"

#define WORLD_X 4
#define WORLD_Z 4

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

	//Chunk m_chunks[WORLD_X][WORLD_Z];
	Chunk *m_chunks;
};


#endif
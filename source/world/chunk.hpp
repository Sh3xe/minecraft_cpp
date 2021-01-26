#ifndef CHUNK_INCLUDED_
#define CHUNK_INCLUDED_

// TEMPORAIRE, A TERME, LA TEXTURE ET LE SHADER SERA DANS L'OBJET "World"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"

#define CHUNK_X 16
#define CHUNK_Y 64
#define CHUNK_Z 16

class Camera;

class Chunk {
public:
	Chunk();

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);

	void draw( Camera &camera );
	//void update( double delta_time );

private:
	void generateMesh();
	void generateTerrain();

	Shader m_shader;
	Texture m_tilset;

	int m_element_count = 0;
	int m_face_count = 0;
	bool m_should_update = true;
	
	unsigned m_vao, m_vbo;
	unsigned char m_block_data[CHUNK_X][CHUNK_Z][CHUNK_Y];

};


#endif
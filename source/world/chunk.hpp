#ifndef CHUNK_INCLUDED_
#define CHUNK_INCLUDED_

#include <glm/glm.hpp>

#define CHUNK_X 16
#define CHUNK_Y 64
#define CHUNK_Z 16

class Camera;
class World;
class Texture;
class Shader;

class Chunk {
public:
	Chunk();
	Chunk(int x, int y, int z, World* World);

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);

	void draw( Camera &camera, Texture &tileset, Shader &shader );
	//void update( double delta_time );

private:
	void generateMesh();
	void generateTerrain();

	World *m_world = nullptr;

	int m_element_count = 0;
	int m_face_count = 0;
	bool m_should_update = true;
	
	unsigned m_vao, m_vbo;
	unsigned char m_block_data[CHUNK_X][CHUNK_Z][CHUNK_Y];

	glm::ivec3 m_position;

};


#endif
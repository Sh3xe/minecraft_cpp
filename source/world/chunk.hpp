#ifndef CHUNK_INCLUDED_
#define CHUNK_INCLUDED_

#include <glm/glm.hpp>
#include <memory>

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
	Chunk(int x, int y, int z);

	void setPosition(int x, int y, int z);
	void setNeighbours(Chunk *px, Chunk *mx, Chunk *py, Chunk *my);
	void generateTerrain();

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);

	void draw( Camera &camera, Texture &tileset, Shader &shader );

private:
	void generateMesh();

	int m_element_count = 0;
	int m_face_count = 0;
	bool m_should_update = true;
	
	unsigned m_vao, m_vbo;
	unsigned char m_block_data[CHUNK_X][CHUNK_Z][CHUNK_Y];
	Chunk *m_neighbours[4];

	glm::ivec3 m_position;

};


#endif
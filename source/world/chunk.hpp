#ifndef CHUNK_INCLUDED_
#define CHUNK_INCLUDED_

#include <glm/glm.hpp>
#include <array>
#include "block.hpp"

#define CHUNK_X 16
#define CHUNK_Y 64
#define CHUNK_Z 16

class Camera;
class Texture;
class Shader;
class PerlinNoise;
class World;

class Chunk {
public:
	Chunk();
	Chunk(int x, int z);
	~Chunk();

	void setPosition(int x, int z);
	void setNeighbours(Chunk *px, Chunk *mx, Chunk *py, Chunk *my);
	void generateTerrain(PerlinNoise &noise_generator);
	void generateMesh();

	void setBlock(int x, int y, int z, unsigned char type);
	unsigned char getBlock(int x, int y, int z);
	glm::ivec2 getPosition() {return m_position;}

	void draw( Camera &camera, Texture &tileset, Shader &shader );

	bool m_should_update = true;

private:
	void plantTree(int x, int y, int z);

	glm::ivec2 m_position;

	int m_element_count = 0;
	int m_face_count = 0;
	
	unsigned m_vao, m_vbo;

	std::array<std::array<std::array<unsigned char, CHUNK_Y>, CHUNK_Z>, CHUNK_X> m_block_data;
	std::array<Chunk*, 4> m_neighbours;


};


#endif
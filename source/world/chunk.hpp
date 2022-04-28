#pragma once

#include <glm/glm.hpp>
#include <array>

#include "block.hpp"
#include "../opengl_wrapper/chunk_mesh.hpp"

constexpr int CHUNK_X{ 16 };
constexpr int CHUNK_Y{ 64 };
constexpr int CHUNK_Z{ 16 };

class Camera;
class Texture;
class Shader;
class PerlinNoise;
class World;

class Chunk {
public:
	friend class TerrainGenerator;

	Chunk();
	Chunk(int x, int z);
	~Chunk();

	void setPosition(int x, int z);
	void setNeighbours(Chunk *px, Chunk *mx, Chunk *py, Chunk *my);
	void set_block(int x, int y, int z, unsigned char type);
	unsigned char set_block(int x, int y, int z);
	glm::ivec2 getPosition() {return m_position;}
	void generateMesh();
	void draw( Camera& camera, Texture& tileset, Shader& shader );

public:
	bool m_should_update = true;

private:
	uint8_t fast_get(int x, int y, int z);
	void fast_set(int x, int y, int z, uint8_t block);

	glm::ivec2 m_position;
	ChunkMesh m_mesh;

	std::array<unsigned char, CHUNK_X* CHUNK_Y* CHUNK_Z> m_block_data;
	std::array<Chunk*, 4> m_neighbours;

};
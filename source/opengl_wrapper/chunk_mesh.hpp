#pragma once

#include "vertex.hpp"
#include "../world/block.hpp"

#include <cstdint>
#include <vector>

class Camera;
class Texture;
class Shader;

enum class Directions: uint8_t
{
	PX = 0,
	MX = 1,
	PY = 2,
	MY = 3,
	PZ = 4,
	MZ = 5
};

class ChunkMesh
{
public:
	ChunkMesh();
	ChunkMesh( int x, int y );
	~ChunkMesh();

	void clear();
	void add_face(int x, int y, int z, Directions dir, BlockID block);
	void send_to_gpu();
	inline uint32_t get_face_count() const { return m_face_count; }
	void render(Camera& camera, Texture& tileset, Shader& shader);

public:
	int x {0}, y{0};

private:
	std::vector<BlockVertex> m_vertices;
	uint32_t m_face_count {0};
	uint32_t m_vao, m_vbo;
};
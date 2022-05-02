#pragma once

#include "vertex.hpp"
#include "world/blocks.hpp"

#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

class Camera;
class Texture;
class Shader;

enum class Directions: uint8_t
{
	px = 0,
	mx = 1,
	py = 2,
	my = 3,
	pz = 4,
	mz = 5
};

class ChunkMesh
{
public:
	ChunkMesh();
	ChunkMesh( int x, int y, bool transparency = false );
	~ChunkMesh();

	void clear();
	void add_face( int x, int y, int z, Directions dir, const BlockType &block );
	void send_to_gpu();
	inline uint32_t get_face_count() const { return m_face_count; }
	void render( Texture& tileset, Shader& shader );
	// tri les triangles du plus loin au plus proche par rapport a [x, y, z]
	void sort_faces_from_distance( const glm::vec3 &pos );

public:
	int x {0}, y{0};
	bool transparency {false};

private:
	std::vector<BlockVertex> m_vertices;
	uint32_t m_face_count {0};
	uint32_t m_vao, m_vbo;
};
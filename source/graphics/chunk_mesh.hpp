#pragma once

#include "vertex.hpp"
#include "world/blocks.hpp"
#include "mesh.hpp"

#include <glm/vec3.hpp>
#include <cstdint>
#include <vector>

class Camera;
class Texture;
class Shader;

enum class Direction: uint8_t
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
	friend class Renderer;

	ChunkMesh() {}
	ChunkMesh( uint32_t x, uint32_t y ): m_x(x), m_y(y) {}

	void clear();
	void send_to_gpu();

	void add_face( int x, int y, int z, Direction dir, const blk::BlockData &block );
	void add_x_shape( int x, int y, int z, const blk::BlockData &block );

	void deb() {m_block_mesh.deb();}

private:
	uint32_t m_x = 0, m_y = 0;

	Mesh<Vertex4Byte> m_block_mesh;
	Mesh<Vertex4Byte> m_water_mesh;
	Mesh<Vertex4Byte> m_foliage_mesh;
};
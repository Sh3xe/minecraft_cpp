#pragma once

#include <cstdint>
#include "vertex.hpp"
#include "world/blocks.hpp"
#include "chunk_mesh.hpp"
#include "gl_functions.hpp"

template <typename VertexType>
struct Mesh
{
	Mesh()
	{
		glCreateVertexBuffers(1, &vbo);
		glCreateVertexArrays(1, &vao);
	}

	~Mesh()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void clear() { vertices.clear(); }

	int x {0}, y{0};
	std::vector<VertexType> vertices;
	uint32_t face_cout {0};
	uint32_t vao, vbo;
};

template<typename VertexType>
void add_face( Mesh<VertexType> &mesh, Direction dir, blk::BlockData &blk );

template<typename VertexType>
void upload( Mesh<VertexType> &mesh );
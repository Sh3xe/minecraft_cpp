#pragma once

#include <glad/glad.h>
#include <cstdint>
#include <vector>

#include "vertex.hpp"
#include "utils.hpp"
#include "core/logger.hpp"

template <typename VertexType>
class Mesh: public no_copy
{
public:
	friend class Renderer;

	Mesh()
	{
		glGenBuffers(1, &m_vbo);
		glGenVertexArrays(1, &m_vao);
	}

	~Mesh()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}

	inline void push( const VertexType &vertex )
	{
		m_vertices.push_back(vertex);
	}

	inline void push( const std::vector<VertexType> &vertices )
	{
		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	}

	inline void clear() { m_vertices.clear(); }

	void upload()
	{
		static_assert( is_vertex_type<VertexType>() );
		//TODO: essayer de ne pas bind le buffer pour voir s'il est bien référencé par le vao
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		//TODO: faire en sorte que ça fonctionne pour Vertex8Byte
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(VertexType), (void*)(0));
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void deb() {SD_TRACE(m_vertices.size());}

private:
	std::vector<VertexType> m_vertices;
	uint32_t m_vao, m_vbo;
	bool m_cull;
};
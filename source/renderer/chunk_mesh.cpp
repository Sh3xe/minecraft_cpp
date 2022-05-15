#include "chunk_mesh.hpp"

#include "texture.hpp"
#include "shader.hpp"
#include "core/camera.hpp"
#include "core/logger.hpp"
#include <cassert>

#include "gl_functions.hpp"

static const int faces[6][6][6] =
{
	{
		{1, 0, 0, 0, 1, 0}, // x, y, tx, ty, num
		{1, 1, 1, 1, 0, 0}, 
		{1, 0, 1, 1, 1, 0}, 
		{1, 0, 0, 0, 1, 0}, 
		{1, 1, 0, 0, 0, 0}, 
		{1, 1, 1, 1, 0, 0}
	}, {
		{0, 0, 1, 0, 1, 1},
		{0, 1, 0, 1, 0, 1},
		{0, 0, 0, 1, 1, 1},
		{0, 0, 1, 0, 1, 1},
		{0, 1, 1, 0, 0, 1},
		{0, 1, 0, 1, 0, 1}
	}, {
		{0, 1, 0, 0, 1, 2},
		{1, 1, 1, 1, 0, 2},
		{1, 1, 0, 1, 1, 2},
		{0, 1, 1, 0, 0, 2},
		{1, 1, 1, 1, 0, 2},
		{0, 1, 0, 0, 1, 2}
	}, {
		{0, 0, 0, 1, 1, 3},
		{1, 0, 0, 0, 1, 3},
		{1, 0, 1, 0, 0, 3},
		{0, 0, 0, 1, 1, 3},
		{1, 0, 1, 0, 0, 3},
		{0, 0, 1, 1, 0, 3}
	}, {
		{1, 0, 1, 0, 1, 4},
		{0, 1, 1, 1, 0, 4},
		{0, 0, 1, 1, 1, 4},
		{1, 1, 1, 0, 0, 4},
		{0, 1, 1, 1, 0, 4},
		{1, 0, 1, 0, 1, 4}
	}, {
		{0, 0, 0, 0, 1, 5},
		{1, 1, 0, 1, 0, 5},
		{1, 0, 0, 1, 1, 5},
		{0, 1, 0, 0, 0, 5},
		{1, 1, 0, 1, 0, 5},
		{0, 0, 0, 0, 1, 5}
	}
};

static const int xfaces[2][6][6] = 
{
	{
		{0, 0, 0, 1, 1, 6}, // x, y, tx, ty, num
		{1, 0, 1, 0, 1, 6}, 
		{1, 1, 1, 0, 0, 6}, 
		{0, 0, 0, 1, 1, 6}, 
		{1, 1, 1, 0, 0, 6}, 
		{0, 1, 0, 1, 0, 6}
	}, {
		{1, 0, 0, 1, 1, 7},
		{0, 0, 1, 0, 1, 7},
		{0, 1, 1, 0, 0, 7},
		{1, 0, 0, 1, 1, 7},
		{0, 1, 1, 0, 0, 7},
		{1, 1, 0, 1, 0, 7}
	}
};

ChunkMesh::ChunkMesh()
{
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
}

ChunkMesh::ChunkMesh(int x, int y, bool transparency ):
	ChunkMesh()
{
	transparency = transparency;
	this->x = x;
	this->y = y;
}

ChunkMesh::~ChunkMesh()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void ChunkMesh::clear()
{
	m_vertices.clear();
}

void ChunkMesh::add_face( int x, int y, int z, Directions dir, const blk::BlockData &block )
{	
	++m_face_count;
	int texture_x{ block.faces[static_cast<uint8_t>(dir)] % 16 };
	int texture_y{ block.faces[static_cast<uint8_t>(dir)] / 16 };

	for (int i = 0; i < 6; ++i)
	{
		auto model = faces[static_cast<uint8_t>(dir)][i];
		m_vertices.emplace_back( x + model[0], y + model[1], z + model[2], texture_x + model[3], texture_y + model[4], model[5] );
	}
}

void ChunkMesh::add_x_shape( int x, int y, int z, const blk::BlockData &block )
{
	assert( block.shape == blk::BlockShape::x );
	m_face_count += 2;

	int texture_x{ block.faces[0] % 16 };
	int texture_y{ block.faces[0] / 16 };

	for( int i = 0; i < 2; ++i )
	for( int j = 0; j < 6; ++j )
	{
		auto model = xfaces[i][j];
		m_vertices.emplace_back( x + model[0], y + model[1], z + model[2], texture_x + model[3], texture_y + model[4], model[5] );
	}

}

void ChunkMesh::send_to_gpu()
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(BlockVertex), m_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribIPointer(0, 1, GL_INT, GL_FALSE, nullptr);
	glEnableVertexAttribArray(0);
}

void ChunkMesh::sort_faces_from_distance( const glm::vec3 &pos )
{

}


void ChunkMesh::render( Texture& tileset, Shader& shader )
{
	if (m_vertices.size() == 0) return;

	shader.bind();
	glBindVertexArray(m_vao);

	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tileset.get_id());
	shader.set_int("u_texture", 0);
	shader.set_vec3("chunk_position", this->x, 0, this->y);
	
	if( transparency )
	{
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glBlendEquation( GL_FUNC_ADD );
	}
	else
	{
		glDisable( GL_BLEND );
	}

	if( cull )
		glEnable( GL_CULL_FACE );
	else
		glDisable( GL_CULL_FACE );

	glDrawArrays(GL_TRIANGLES, 0, m_face_count * 6);
}
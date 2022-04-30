#include "chunk_mesh.hpp"

#include "texture.hpp"
#include "shader.hpp"
#include "../core/camera.hpp"

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

ChunkMesh::ChunkMesh()
{
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
}

ChunkMesh::ChunkMesh(int x, int y):
	ChunkMesh()
{
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

void ChunkMesh::add_face(int x, int y, int z, Directions dir, BlockID block)
{
	++m_face_count;
	
	auto &db = BlockDB::get();

	int texture_x{ db.id_get(block).faces[static_cast<uint8_t>(dir)] % 16 };
	int texture_y{ db.id_get(block).faces[static_cast<uint8_t>(dir)] / 16 };

	for (int i = 0; i < 6; ++i)
	{
		auto model = faces[static_cast<uint8_t>(dir)][i];
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

void ChunkMesh::render(Camera& camera, Texture& tileset, Shader& shader)
{
	if (m_vertices.size() == 0) return;

	shader.bind();
	glBindVertexArray(m_vao);

	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tileset.get_id());
	shader.set_int("u_texture", 0);

	// matrix magic
	shader.set_vec3("chunk_position", this->x, 0, this->y);

	glDrawArrays(GL_TRIANGLES, 0, m_face_count * 6);
}
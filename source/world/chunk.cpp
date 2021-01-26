#include "chunk.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <iostream>

#include "../core/camera.hpp"
#include "../opengl_wrapper/vertex.hpp"
#include "../opengl_wrapper/gldebug.hpp"
#include "block.hpp"

using byte4 = glm::tvec4<GLbyte>;

Chunk::Chunk():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl"),
	m_tilset("resources/images/tileset.png") {
	
	// generate buffers, give to the shaders matrices
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(3.141592853f / 2.f, 16.f / 9.f, .01f, 100.f);

	m_shader.use();
	//m_shader.setMat4("model", glm::value_ptr(model_matrix));
	m_shader.setMat4("projection", glm::value_ptr(projection_matrix));

	// generate terrain
	generateTerrain();

}

void Chunk::setBlock(int x, int y, int z, unsigned char type) {
	m_block_data[x][z][y] = type;
	m_should_update = true;
}

unsigned char Chunk::getBlock(int x, int y, int z) {
	if (x >= CHUNK_X || x < 0 || y >= CHUNK_Y || y < 0 || z >= CHUNK_Z || z < 0)
		return 0;
	return m_block_data[x][z][y];
}

void Chunk::generateMesh() {
	int count = m_element_count * 36;
	m_face_count = 0;

	Vertex *vertices = new Vertex[count];
	int i = 0;

	for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z)
			for(int y = 0; y < CHUNK_Y; ++y) {
				unsigned char type = getBlock(x, y, z);
				if(type == Blocks::AIR) continue;

				// +x
				if (!getBlock(x + 1, y, z)) {
					int texture_x = Blocks::block_faces[type][0] % 16;
					int texture_y = Blocks::block_faces[type][0] / 16;
					vertices[i++] = Vertex{ x+1, y  , z  , texture_x + 0, texture_y + 1, 230 };
					vertices[i++] = Vertex{ x+1, y+1, z+1, texture_x + 1, texture_y + 0, 230 };
					vertices[i++] = Vertex{ x+1, y  , z+1, texture_x + 1, texture_y + 1, 230 };
					vertices[i++] = Vertex{ x+1, y  , z  , texture_x + 0, texture_y + 1, 230 };
					vertices[i++] = Vertex{ x+1, y+1, z  , texture_x + 0, texture_y + 0, 230 };
					vertices[i++] = Vertex{ x+1, y+1, z+1, texture_x + 1, texture_y + 0, 230 };
					++m_face_count;
				}

				// -x
				if (!getBlock(x - 1, y, z)) {
					int texture_x = Blocks::block_faces[type][1]%16;
					int texture_y = Blocks::block_faces[type][1]/16;
					vertices[i++] = Vertex (x, y  , z+1, texture_x+0, texture_y+1, 230);
					vertices[i++] = Vertex (x, y+1, z  , texture_x+1, texture_y+0, 230);
					vertices[i++] = Vertex (x, y  , z  , texture_x+1, texture_y+1, 230);
					vertices[i++] = Vertex (x, y  , z+1, texture_x+0, texture_y+1, 230);
					vertices[i++] = Vertex (x, y+1, z+1, texture_x+0, texture_y+0, 230);
					vertices[i++] = Vertex (x, y+1, z  , texture_x+1, texture_y+0, 230);
					++m_face_count;
				}

				// +y
				if (!getBlock(x, y + 1, z)) {
					int texture_x = Blocks::block_faces[type][2] % 16;
					int texture_y = Blocks::block_faces[type][2] / 16;
					vertices[i++] = Vertex{ x  , y + 1, z  , texture_x + 0, texture_y + 1, 255 };
					vertices[i++] = Vertex{ x + 1, y + 1, z + 1, texture_x + 1, texture_y + 0, 255 };
					vertices[i++] = Vertex{ x + 1, y + 1, z  , texture_x + 1, texture_y + 1, 255 };
					vertices[i++] = Vertex{ x  , y + 1, z + 1, texture_x + 0, texture_y + 0, 255 };
					vertices[i++] = Vertex{ x + 1, y + 1, z + 1, texture_x + 1, texture_y + 0, 255 };
					vertices[i++] = Vertex{ x  , y + 1, z  , texture_x + 0, texture_y + 1, 255 };
					++m_face_count;
				}

				// -y
				if (!getBlock(x, y-1, z)) {
					int texture_x = Blocks::block_faces[type][3]%16;
					int texture_y = Blocks::block_faces[type][3]/16;
					vertices[i++] = Vertex {x  , y  , z  , texture_x+0, texture_y+0, 200};
					vertices[i++] = Vertex {x+1, y  , z+1, texture_x+1, texture_y+1, 200};
					vertices[i++] = Vertex {x+1, y  , z  , texture_x+0, texture_y+1, 200};
					vertices[i++] = Vertex {x  , y  , z+1, texture_x+1, texture_y+0, 200};
					vertices[i++] = Vertex {x+1, y  , z+1, texture_x+1, texture_y+1, 200};
					vertices[i++] = Vertex {x  , y  , z  , texture_x+0, texture_y+0, 200};
					++m_face_count;
				}
				

				// +z
				if (!getBlock(x, y, z-1)) {
					int texture_x = Blocks::block_faces[type][4] % 16;
					int texture_y = Blocks::block_faces[type][4] / 16;
					vertices[i++] = Vertex{ x  , y  , z  , texture_x + 0, texture_y + 1, 200 };
					vertices[i++] = Vertex{ x+1, y+1, z  , texture_x + 1, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x+1, y  , z  , texture_x + 1, texture_y + 1, 200 };
					vertices[i++] = Vertex{ x  , y+1, z  , texture_x + 0, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x+1, y+1, z  , texture_x + 1, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x  , y  , z  , texture_x + 0, texture_y + 1, 200 };
					++m_face_count;
				}

				// -z
				if (!getBlock(x, y, z+1)) {
					int texture_x = Blocks::block_faces[type][5] % 16;
					int texture_y = Blocks::block_faces[type][5] / 16;
					vertices[i++] = Vertex{ x+1, y  , z+1, texture_x + 0, texture_y + 1, 200 };
					vertices[i++] = Vertex{ x  , y+1, z+1, texture_x + 1, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x  , y  , z+1, texture_x + 1, texture_y + 1, 200 };
					vertices[i++] = Vertex{ x+1, y+1, z+1, texture_x + 0, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x  , y+1, z+1, texture_x + 1, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x+1, y  , z+1, texture_x + 0, texture_y + 1, 200 };
					++m_face_count;
				}

			}
	
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(3*sizeof(unsigned char)));
	glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(5*sizeof(unsigned char)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	
	logGlErrors();
	m_should_update = false;
	delete[] vertices;
}

void Chunk::generateTerrain() {
	m_element_count = 0;

	for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z)
			for(int y = 0; y < CHUNK_Y; ++y) {
				if(y < 16) {
					m_block_data[x][z][y] = Blocks::STONE;
					++m_element_count;
				} else if (y <= 19) {
					m_block_data[x][z][y] = Blocks::DIRT;
					++m_element_count;
				} else if (y == 20) {
					m_block_data[x][z][y] = Blocks::GRASS;
					++m_element_count;
				} else m_block_data[x][z][y] = Blocks::AIR;
			}

	/*
	for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z)
			for(int y = 0; y < CHUNK_Y; ++y)
				m_block_data[x][z][y] = Blocks::AIR;
	m_block_data[0][0][0] = Blocks::GRASS;
	m_element_count = 1;
	*/
}

void Chunk::draw( Camera &camera ) {
	if(m_should_update)
		generateMesh();
	
	if(m_element_count) {
		m_shader.use();
		glBindVertexArray(m_vao);

		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tilset.getId());
		m_shader.setInt("u_texture", 0);

		// matrix magic
		glm::mat4 view_matrix = camera.getViewMatrix();
		m_shader.setMat4("view", glm::value_ptr(view_matrix));

		glDrawArrays(GL_TRIANGLES, 0, m_face_count * 6);
	}
}

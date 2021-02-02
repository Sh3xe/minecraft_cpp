#include "chunk.hpp"

#include <glad/glad.h>
#include <random>
#include <iostream>

#include "../core/perlin.hpp"
#include "../core/camera.hpp"
#include "../opengl_wrapper/vertex.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"

Chunk::Chunk(int x, int z):
	m_position(x, z),
	m_neighbours({nullptr, nullptr , nullptr , nullptr }) {

	// fill blocks array
	for(int x = 0; x < CHUNK_X; ++x)
	for(int y = 0; y < CHUNK_Y; ++y)
	for(int z = 0; z < CHUNK_Z; ++z)
		m_block_data[x][z][y] = Blocks::AIR;

	// generate buffers, give to the shaders matrices
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
}

Chunk::Chunk() :
	Chunk(0, 0) {
}

void Chunk::setPosition(int x, int z) {
	m_position.x = x;
	m_position.y = z;
}

void Chunk::setNeighbours(Chunk *px, Chunk *mx, Chunk *pz, Chunk *mz) {
	// set neighbour's chunks (used for block checking)
	m_neighbours[0] = px;
	m_neighbours[1] = mx;
	m_neighbours[2] = pz;
	m_neighbours[3] = mz;
}


void Chunk::setBlock(int x, int y, int z, unsigned char type) {
	m_block_data[x][z][y] = type;
	m_should_update = true;

	if(type == Blocks::AIR) { // we might need to update nearby chunks to make sure there is no hole when breaking blocks
		if( x == CHUNK_X - 1 ) m_neighbours[0]->m_should_update = true;
		if( x == 0 ) m_neighbours[1]->m_should_update = true;
		if( z == CHUNK_Z - 1 ) m_neighbours[2]->m_should_update = true;
		if( z == 0 ) m_neighbours[3]->m_should_update = true;
	}
}

unsigned char Chunk::getBlock(int x, int y, int z) {
	// warning: only works for 1 block out-of-range.
	// if x, y, z are out of range, we return the right block in the neighbour's chunk
	if(y >= CHUNK_Y)
		return Blocks::AIR;
	if(y < 0)
		return Blocks::STONE;
	if(x >= CHUNK_X)
		return m_neighbours[0] != nullptr ? m_neighbours[0]->getBlock(0, y, z): Blocks::STONE;
	if(x < 0)
		return m_neighbours[1] != nullptr ? m_neighbours[1]->getBlock(CHUNK_X - 1, y, z): Blocks::STONE;
	if(z >= CHUNK_Z)
		return m_neighbours[2] != nullptr ? m_neighbours[2]->getBlock(x, y, 0): Blocks::STONE;
	if(z < 0)
		return m_neighbours[3] != nullptr ? m_neighbours[3]->getBlock(x, y, CHUNK_Z - 1): Blocks::STONE;

	// return the block in the current chunk
	return m_block_data[x][z][y];
}

void Chunk::generateMesh() {
	// updates the block's vbo.
	int count = m_element_count * 36;
	m_face_count = 0;

	Vertex *vertices = new Vertex[count];
	int i = 0;

	for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z)
			for(int y = 0; y < CHUNK_Y; ++y) {
				// adds a face the the mesh if needed

				unsigned char type = getBlock(x, y, z);
				if(type == Blocks::AIR) continue;

				// +x
				if (!getBlock(x+1, y, z)) {
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
				if (!getBlock(x-1, y, z)) {
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
				if (!getBlock(x, y+1, z)) {
					int texture_x = Blocks::block_faces[type][2] % 16;
					int texture_y = Blocks::block_faces[type][2] / 16;
					vertices[i++] = Vertex{ x  , y+1, z  , texture_x + 0, texture_y + 1, 255 };
					vertices[i++] = Vertex{ x+1, y+1, z+1, texture_x + 1, texture_y + 0, 255 };
					vertices[i++] = Vertex{ x+1, y+1, z  , texture_x + 1, texture_y + 1, 255 };
					vertices[i++] = Vertex{ x  , y+1, z+1, texture_x + 0, texture_y + 0, 255 };
					vertices[i++] = Vertex{ x+1, y+1, z+1, texture_x + 1, texture_y + 0, 255 };
					vertices[i++] = Vertex{ x  , y+1, z  , texture_x + 0, texture_y + 1, 255 };
					++m_face_count;
				}

				// -y
				if (!getBlock(x, y - 1, z)) {
					int texture_x = Blocks::block_faces[type][3]%16;
					int texture_y = Blocks::block_faces[type][3]/16;
					vertices[i++] = Vertex {x  , y  , z  , texture_x+0, texture_y+0, 150};
					vertices[i++] = Vertex {x+1, y  , z+1, texture_x+1, texture_y+1, 150 };
					vertices[i++] = Vertex {x+1, y  , z  , texture_x+0, texture_y+1, 150 };
					vertices[i++] = Vertex {x  , y  , z+1, texture_x+1, texture_y+0, 150 };
					vertices[i++] = Vertex {x+1, y  , z+1, texture_x+1, texture_y+1, 150 };
					vertices[i++] = Vertex {x  , y  , z  , texture_x+0, texture_y+0, 150 };
					++m_face_count;
				}
				
				// +z
				if (!getBlock(x, y, z + 1)) {
					int texture_x = Blocks::block_faces[type][5] % 16;
					int texture_y = Blocks::block_faces[type][5] / 16;
					vertices[i++] = Vertex{ x + 1, y  , z + 1, texture_x + 0, texture_y + 1, 200 };
					vertices[i++] = Vertex{ x  , y + 1, z + 1, texture_x + 1, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x  , y  , z + 1, texture_x + 1, texture_y + 1, 200 };
					vertices[i++] = Vertex{ x + 1, y + 1, z + 1, texture_x + 0, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x  , y + 1, z + 1, texture_x + 1, texture_y + 0, 200 };
					vertices[i++] = Vertex{ x + 1, y  , z + 1, texture_x + 0, texture_y + 1, 200 };
					++m_face_count;
				}

				// -z
				if (!getBlock(x, y, z - 1)) {
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

			}
	
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(unsigned char)));
	glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_TRUE , sizeof(Vertex), (void*)(5*sizeof(unsigned char)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	m_should_update = false;
	delete[] vertices;
}

void Chunk::generateTerrain(PerlinNoise &noise_generator) {
	// shitty, temporary world generation

	m_element_count = 0;
	for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z) {
			int height = 10 + noise_generator.noise(0.1 * (m_position.x + x), 0.1 * (m_position.y + z)) * 10;
			if (height < 0) height = 0;
			//int height = 10;
			for(int y = 0; y < CHUNK_Y; ++y) {
				if( y <= height ) {
					if (y == height)
						setBlock(x, y, z, Blocks::GRASS);
					else if(y > height - 3 )
						setBlock(x, y, z, Blocks::DIRT);
					else
						setBlock(x, y, z, Blocks::STONE);
					++m_element_count;
				}
			}
		}
}

void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader ) {
	if(m_should_update)
		generateMesh();
	
	if(m_element_count) {
		shader.use();
		glBindVertexArray(m_vao);

		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tileset.getId());
		shader.setInt("u_texture", 0);

		// matrix magic
		shader.setVec3("chunk_position", m_position.x, 0, m_position.y);

		glDrawArrays(GL_TRIANGLES, 0, m_face_count * 6);
	}
}

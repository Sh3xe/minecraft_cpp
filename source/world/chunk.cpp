#include "chunk.hpp"

#include <glad/glad.h>
#include <random>
#include <iostream>

#include "world.hpp"
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

Chunk::~Chunk() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
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

	/*
	for(Chunk *neighbour: m_neighbours)
		if( neighbour != nullptr )
			neighbour->m_should_update = true;*/
}


void Chunk::setBlock(int x, int y, int z, unsigned char type) {

	if (x >= CHUNK_X && m_neighbours[0] != nullptr)
		m_neighbours[0]->setBlock(0, y, z, type);
	if (x < 0 && m_neighbours[1] != nullptr)
		m_neighbours[1]->setBlock(CHUNK_X - 1, y, z, type);
	if (z >= CHUNK_Z && m_neighbours[2] != nullptr)
		m_neighbours[2]->setBlock(x, y, 0, type);
	if (z < 0 && m_neighbours[3] != nullptr)
		m_neighbours[3]->setBlock(x, y, CHUNK_Z - 1, type);

	if (x >= 0 && x < CHUNK_X && y >= 0 && y < CHUNK_Y && z >= 0 && z < CHUNK_Z) {
		m_block_data[x][z][y] = type;
		m_should_update = true;

		
		if (x == CHUNK_X - 1 && m_neighbours[0] != nullptr) m_neighbours[0]->m_should_update = true;
		if (x == 0 && m_neighbours[1] != nullptr) m_neighbours[1]->m_should_update = true;
		if (z == CHUNK_Z - 1 && m_neighbours[2] != nullptr) m_neighbours[2]->m_should_update = true;
		if (z == 0 && m_neighbours[3] != nullptr) m_neighbours[3]->m_should_update = true;
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
	static std::default_random_engine e;
	static std::bernoulli_distribution d(0.8);
	static std::bernoulli_distribution dt(0.03);

	m_element_count = 0;
	for (int x = 0; x < CHUNK_X; ++x)
	for (int z = 0; z < CHUNK_Z; ++z) {
		int height_value = 30 + (noise_generator.noise(m_position.x + x, m_position.y + z, 0.05) * 2 + noise_generator.noise(m_position.x + x, m_position.y + z, 0.01)) * 5;

		for (int y = 0; y < CHUNK_Y; ++y) {
			if (y <= height_value) {
				if (y == height_value) {
					setBlock(x, y, z, Blocks::GRASS);
				} else if (y > height_value - 3) {
					setBlock(x, y, z, Blocks::DIRT);
				} else {
					if (dt(e)) {
						setBlock(x, y, z, Blocks::GOLD_ORE);
					} else if (d(e))
						setBlock(x, y, z, Blocks::STONE);
					else
						setBlock(x, y, z, Blocks::COBBLESTONE);
				}
				++m_element_count;
			}
		}}
}

void Chunk::plantTree(int x, int y, int z) {
	// plant a tree by looping over an array that contain tree's information and placing blocks
	for (int i = -1; i <= 1; ++i)
	for (int j = -1; j <= 1; ++j)
	for (int k = 0; k <= 3 ; ++k) {
		Blocks::BlockType type = Blocks::tree_blocks[k * 9 + (i + 1) + (j + 1) * 3];
		if (type != Blocks::AIR) {
			setBlock(i + x, k + y, j + z, type);
			++m_element_count;
		}
	}
}


void Chunk::draw( Camera &camera, Texture &tileset, Shader &shader ) {
	if( m_should_update ) 
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

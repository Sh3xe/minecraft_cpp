#include "chunk.hpp"

#include <glad/glad.h>
#include <iostream>

#include "world.hpp"
#include "../core/perlin.hpp" // not my implementation
#include "../core/camera.hpp"
#include "../opengl_wrapper/vertex.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"
#include "../opengl_wrapper/gldebug.hpp"

#include "block.hpp"

Chunk::Chunk(int x, int y, int z, World *world):
	m_position(x, y, z),
	m_world(world),
	m_block_data(new unsigned char[CHUNK_X * CHUNK_Y * CHUNK_Z]) {

	// generate buffers, give to the shaders matrices
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
}

Chunk::Chunk() :
	Chunk(0, 0, 0, nullptr) {
}

void Chunk::setPosition(int x, int y, int z) {
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void Chunk::setWorld(World* world) {
	m_world = world;
}

void Chunk::setBlock(int x, int y, int z, unsigned char type) {
	m_block_data[x * CHUNK_Y * CHUNK_Z + y * CHUNK_Z + z] = type;
	m_should_update = true;
}

unsigned char Chunk::getBlock(int x, int y, int z) {
	// no need this test right now since it will be used by the world class
	if (x >= CHUNK_X || x < 0 || y >= CHUNK_Y || y < 0 || z >= CHUNK_Z || z < 0)
		return Blocks::AIR;

	return m_block_data[x * CHUNK_Y * CHUNK_Z + y * CHUNK_Z + z];
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
				//if (!m_world->getBlock( m_position.x + x + 1, m_position.y + y, m_position.z + z)) {
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
				//if (!m_world->getBlock(m_position.x + x - 1, m_position.y + y, m_position.z + z)) {
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
				//if (!m_world->getBlock(m_position.x + x, m_position.y + y + 1, m_position.z + z)) {
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
				//if (!m_world->getBlock(m_position.x + x, m_position.y + y - 1, m_position.z + z)) {
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
				//if (!m_world->getBlock(m_position.x + x, m_position.y + y, m_position.z + z + 1)) {
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
				//if (!m_world->getBlock(m_position.x + x, m_position.y + y, m_position.z + z - 1)) {
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

	
	logGlErrors();
	m_should_update = false;
	delete[] vertices;
}

void Chunk::generateTerrain() {
	static PerlinNoise pn(250);

	m_element_count = 0;
	for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z) {
			int height_value = 20 + (int)(pn.noise((m_position.x + x) * 0.01f, (m_position.z +z) * 0.01f) * 10) + pn.noise((m_position.x + x) * 0.1f, (m_position.z +z) * 0.1f) * 15;
			for(int y = 0; y < CHUNK_Y; ++y) {
				if (y <= height_value) {
					if (y == height_value) setBlock(x, y, z, Blocks::GRASS);
					else if (y < height_value && y > height_value - 4) setBlock(x, y, z, Blocks::DIRT);
					else if (y <= height_value -4) setBlock(x, y, z, Blocks::STONE);

					++m_element_count;
				}
				else
					setBlock(x, y, z, Blocks::AIR);
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
		shader.setVec3("chunk_position", m_position.x, m_position.y, m_position.z);

		glDrawArrays(GL_TRIANGLES, 0, m_face_count * 6);
	}
}

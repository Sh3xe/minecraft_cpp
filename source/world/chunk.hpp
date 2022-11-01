#pragma once

#include <glm/glm.hpp>
#include <utility>
#include <array>
#include <atomic>

#include "blocks.hpp"
#include "graphics/chunk_mesh.hpp"

constexpr int CHUNK_SIDE{ 16 };
constexpr int CHUNK_HEIGHT{ 185 };

enum class ChunkState
{
	ready,
	need_generation,
	need_mesh_update,
	need_upload
};

struct Chunk
{
	Chunk(int x, int y): position(x, y) {}

	void set_block(int x, int y, int z, blk::BlockType type);
	blk::BlockType get_block(int x, int y, int z);
	
	blk::BlockType fast_get(int x, int y, int z);
	void fast_set(int x, int y, int z, blk::BlockType block);

	void construct_mesh();

	std::atomic< ChunkState > state { ChunkState::need_generation };
	glm::ivec2 position;
	std::array< blk::BlockType, CHUNK_SIDE * CHUNK_HEIGHT * CHUNK_SIDE > blocks;
	std::array< Chunk*, 4> neighbours {nullptr, nullptr, nullptr, nullptr};
	std::array< bool, CHUNK_HEIGHT> layers;

	ChunkMesh mesh;
};

std::pair<int, int> get_pos_inside_chunk( int x, int z );
std::pair<int, int> get_pos_of_chunk( int x, int z );
#pragma once

#include <glm/glm.hpp>
#include <utility>
#include <array>
#include <atomic>

#include "blocks.hpp"
#include "renderer/chunk_mesh.hpp"
#include "renderer/mesh.hpp"
#include "chunk.hpp"

constexpr int CHUNK_SIDE{ 16 };
constexpr int CHUNK_HEIGHT{ 185 };

enum class ChunkState
{
	ready,
	need_generation,
	need_mesh_update,
	need_upload
};

class Camera;
class Texture;
class Shader;
class SimplexNoise;
class World;

struct ChunkData
{
	void set_block(int x, int y, int z, blk::BlockType type);
	blk::BlockType get_block(int x, int y, int z);
	
	blk::BlockType fast_get(int x, int y, int z);
	void fast_set(int x, int y, int z, blk::BlockType block);


	std::atomic< ChunkState > state { ChunkState::need_generation };
	glm::ivec2 position;
	std::array< blk::BlockType, CHUNK_SIDE * CHUNK_HEIGHT * CHUNK_SIDE > blocks;
	std::array< ChunkData*, 4> neighbours;
	std::array< bool, CHUNK_HEIGHT> layers;

	Mesh<Vertex4Byte> block_mesh;
	Mesh<Vertex8Byte> water_mesh;
	Mesh<Vertex8Byte> foliage_mesh;
};

void fill_chunk_mesh( ChunkData &data );

std::pair<int, int> get_pos_inside_chunk( int x, int z );
std::pair<int, int> get_pos_of_chunk( int x, int z );
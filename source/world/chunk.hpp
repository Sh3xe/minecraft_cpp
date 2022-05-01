#pragma once

#include <glm/glm.hpp>
#include <utility>
#include <array>
#include <atomic>

#include "blocks.hpp"
#include "renderer/chunk_mesh.hpp"

constexpr int CHUNK_X{ 16 };
constexpr int CHUNK_Y{ 64 };
constexpr int CHUNK_Z{ 16 };

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

class Chunk {
public:
	friend class TerrainGenerator;
	friend class World;

	Chunk( BlockDB &db, int x, int z);
	~Chunk();

	void set_position(int x, int z);
	void set_neighbours(Chunk *px, Chunk *mx, Chunk *py, Chunk *my);
	void set_block(int x, int y, int z, BlockID type);
	BlockID get_block(int x, int y, int z);
	inline glm::ivec2 get_position() const { return m_position; }
	void generate_mesh();
	void draw( Camera &camera, Texture& tileset, Shader& shader );

public:
	std::atomic< ChunkState > state { ChunkState::need_generation };

private:
	uint8_t fast_get(int x, int y, int z);
	void fast_set(int x, int y, int z, uint8_t block);

	glm::ivec2 m_position;
	std::array<ChunkMesh, 3> m_meshes;
	BlockDB *m_db;

	std::array< BlockID, CHUNK_X* CHUNK_Y* CHUNK_Z> m_block_data;
	std::array<Chunk*, 4> m_neighbours;

};


std::pair<int, int> get_pos_inside_chunk( int x, int z );
std::pair<int, int> get_pos_of_chunk( int x, int z );

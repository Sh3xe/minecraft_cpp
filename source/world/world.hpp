#pragma once

#include <map>
#include <memory>
#include <vector>
#include <thread>

#include "core/ts_queue.hpp"
#include "chunk.hpp"
#include "terrain_generator.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture2d.hpp"
#include "math/aabb.hpp"
#include "core/settings.hpp"

class Camera;

class World
{
public:
	friend class TerrainGenerator;

	World( const Settings &settings );
	~World();

	void set_block(int x, int y, int z, blk::BlockType type);
	blk::BlockType get_block(int x, int y, int z);

	void render();
	void update( double delta_time, Camera &camera );
	std::vector<AABB> get_hit_boxes( AABB& box);

private:
	/* update each chunk's neighbour pointers */
	void update_chunk_neighbours();

	/* add all structures blocks waiting to be placed in a chunk */
	void add_blocks(ChunkData&chunk );

	/* to be run by another thread / generate the chunk terrain and mesh */
	void prepare_chunks();

	int m_render_distance { 4 };
	
	TerrainGenerator m_generator;
	ChunkToBePlace m_chunk_blocks; // list of blocks to be placed for a given chunk
	std::map< std::pair<int, int>, std::shared_ptr<ChunkData>> m_chunks;

	std::mutex m_map_mutex;
	std::thread m_worker;
	TSQueue< std::pair<int, int> > m_update_queue;
	std::atomic<bool> m_running {true};
};
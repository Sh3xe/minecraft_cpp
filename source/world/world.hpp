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
#include "core/aabb.hpp"
#include "core/config.hpp"

class Camera;

class World
{
public:
	World( Config &config );
	~World();

	void set_block(int x, int y, int z, BlockID type);
	BlockID get_block(int x, int y, int z);

	void draw( Camera &camera );
	void update( double delta_time, Camera &camera );
	std::vector<AABB> get_hit_boxes( AABB& box);

private:
	/* met à jour les relations de voisins entres tronçons */
	void update_chunk_neighbours();
	void update_neighbours_of( Chunk &chunk, Chunk *ptr );

	/* Ajoute les blocks de structures à chunk */
	void add_blocks( Chunk &chunk );

	/* fonction qui sera lancé par un thread */
	void prepare_chunks();

	int m_render_distance { 3 };
	Shader m_shader;
	Texture2D m_tileset;
	
	BlockDB m_db;
	TerrainGenerator m_generator;
	ChunkToBePlace m_chunk_blocks; // pour un tronçon donnée, la liste des blocks à placer
	std::map< std::pair<int, int>, std::unique_ptr<Chunk>> m_chunks;

	std::mutex m_map_mutex;
	std::thread m_worker;
	TSQueue< std::pair<int, int> > m_update_queue;
	std::atomic<bool> m_running {true};
};
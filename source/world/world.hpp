#pragma once

#include <map>
#include <memory>
#include <vector>

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

	/* Ajoute les blocks de structures à chunk */
	void add_blocks( Chunk &chunk );

	int m_render_distance { 3 };
	Shader m_shader;
	Texture2D m_tileset;
	
	BlockDB m_db;
	TerrainGenerator m_generator;
	ChunkToBePlace m_chunk_blocks; // pour un tronçon donnée, la liste des blocks à placer
	std::map< std::pair<int, int>, std::unique_ptr<Chunk>> m_chunks;
};
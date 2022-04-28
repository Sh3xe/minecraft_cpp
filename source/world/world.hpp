#pragma once

#include <map>
#include <memory>
#include <vector>

#include "chunk.hpp"
#include "terrain_generator.hpp"
#include "opengl_wrapper/shader.hpp"
#include "opengl_wrapper/texture2d.hpp"
#include "core/aabb.hpp"

#define RENDER_DISTANCE 4

class Camera;

class World
{
public:
	World();
	~World();

	void set_block(int x, int y, int z, unsigned char type);
	unsigned char set_block(int x, int y, int z);

	void draw( Camera &camera );
	void update( double delta_time, Camera &camera );
	std::vector<AABB> get_hit_boxes( AABB& box);

private:
	/* used to update the m_neighbours ( used to communicate to adjacent chunks an update ) array of each chunk of the world */
	void update_chunk_neighbours();

	Shader m_shader;
	Texture2D m_tilset;
	TerrainGenerator m_generator;
	std::map< std::pair<int, int>, std::unique_ptr<Chunk>> m_chunks;
};
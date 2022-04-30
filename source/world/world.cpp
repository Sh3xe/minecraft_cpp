#include "world.hpp"

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "utils.hpp"
#include "core/logger.hpp"
#include "core/camera.hpp"
#include "blocks.hpp"

World::World():
	m_shader("resources/shaders/vertex.glsl", "", "resources/shaders/fragment.glsl"),
	m_tilset("resources/images/tileset.png"),
	m_generator( m_db )
{	
	// set shader uniforms
	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix = glm::perspective(3.141592853f / 2.0f, 16.0f / 9.0f, 0.01f, 256.0f);

	m_shader.bind();
	m_shader.set_mat4("projection", glm::value_ptr(projection_matrix));

	if( !m_db.load_blocks_from_file("resources/blocks/blocks.json"))
		SD_ERROR("Impossible de récuperer les infos sur les blocs");

	if( !m_db.load_structures_from_file("resources/blocks/structures.json"))
		SD_ERROR("Impossible de récuperer les infos sur les structures");
}

World::~World()
{
	m_chunks.clear();
}

void World::update_chunk_neighbours()
{
	for(auto it = m_chunks.begin(); it != m_chunks.end(); ++it)
	{
		// set all pointers to nullptr
		Chunk* px = nullptr, * mx = nullptr, * pz = nullptr, * mz = nullptr;

		// try to find the -x neighbour, if found, set "mx" to it
		auto neighbour = m_chunks.find( std::pair<int, int>(it->first.first - 16, it->first.second) );
		if (neighbour != m_chunks.end())
			mx = neighbour->second.get();

		// same for +x
		neighbour = m_chunks.find( std::pair<int, int>(it->first.first + 16, it->first.second) );
		if(neighbour != m_chunks.end())
			px = neighbour->second.get();

		// same for -z
		neighbour = m_chunks.find( std::pair<int, int>(it->first.first, it->first.second - 16) );
		if(neighbour != m_chunks.end())
			mz = neighbour->second.get();

		// same for +z
		neighbour = m_chunks.find( std::pair<int, int>(it->first.first, it->first.second + 16) );
		if(neighbour != m_chunks.end())	
			pz = neighbour->second.get();

		// set the neighbours chunk for this current chunk
		it->second->set_neighbours(px, mx, pz, mz);
	}
}

std::vector<AABB> World::get_hit_boxes( AABB& box)
{
	std::vector<AABB> hitboxes;

	for(int i = floor(box.xmin); i < floor(box.xmax) + 1; i += 1.0)
	for(int j = floor(box.ymin); j < floor(box.ymax) + 1; j += 1.0)
	for(int k = floor(box.zmin); k < floor(box.zmax) + 1; k += 1.0)
	{
		if( m_db.get_block( get_block(i, j, k) ).collidable )
			hitboxes.emplace_back(i, j, k, 1, 1, 1);
	}

	return hitboxes;
}

void World::set_block(int x, int y, int z, BlockID type)
{
	auto [chunk_x, chunk_z] = get_pos_of_chunk(x, z);
	auto [coord_x, coord_z] = get_pos_inside_chunk(x, z);

	auto chunk = m_chunks.find(std::pair<int, int>(chunk_x * 16, chunk_z * 16));

	if( chunk != m_chunks.end() && y < CHUNK_Y && y >= 0 )
		chunk->second->set_block(coord_x, y, coord_z, type);
}

BlockID World::get_block(int x, int y, int z)
{
	auto [chunk_x, chunk_z] = get_pos_of_chunk(x, z);
	auto [coord_x, coord_z] = get_pos_inside_chunk(x, z);

	auto chunk = m_chunks.find(std::pair<int, int>(chunk_x * 16, chunk_z * 16));

	if( chunk != m_chunks.end() && y < CHUNK_Y && y >= 0 )
		return chunk->second->get_block(coord_x, y, coord_z);

	return 0; //__TODO
}

void World::update( double delta_time, Camera &camera ) 
{
	// calculate the position of the chunk ( / 16 )
	
	glm::ivec3 camera_position = camera.get_position();

	int chunk_x = ( static_cast<int>(camera_position.x) / CHUNK_X );
	int chunk_z = ( static_cast<int>(camera_position.z) / CHUNK_Z );

	// remove far away chunks
	for(auto chunk = m_chunks.begin(); chunk != m_chunks.end();) 
	{
		glm::ivec2 chunk_pos = chunk->second->get_position();
		if( // if the cunk is too far
			chunk_pos.x > (chunk_x + RENDER_DISTANCE) * 16 ||
			chunk_pos.x < (chunk_x - RENDER_DISTANCE) * 16 ||
			chunk_pos.y > (chunk_z + RENDER_DISTANCE) * 16 ||
			chunk_pos.y < (chunk_z - RENDER_DISTANCE) * 16
			) 
		{
			// erase, delete, kill
			chunk = m_chunks.erase(chunk);
		}
		else
			++chunk;
	}

	// add new chunk if needed
	// keep track of added chunks
	std::vector < decltype(m_chunks.begin()) > added_chunks;

	// add a chunk if needed
	for (int i = chunk_x - RENDER_DISTANCE; i <= chunk_x + RENDER_DISTANCE; ++i)
	for (int j = chunk_z - RENDER_DISTANCE; j <= chunk_z + RENDER_DISTANCE; ++j) 
	{
		auto it = m_chunks.find(std::pair<int, int>(i * 16, j * 16));
		if (it == m_chunks.end()) 
		{
			auto new_chunk = m_chunks.insert( std::pair< std::pair<int, int>, std::unique_ptr<Chunk>>(
				std::pair<int, int>(i * 16, j * 16), std::unique_ptr<Chunk>(new Chunk(m_db, i * 16, j * 16))
			)).first;

			added_chunks.push_back(new_chunk);
		}
	}
		
	update_chunk_neighbours();

	for (auto chunk : added_chunks)
		m_generator.generate(*chunk->second);
	
}

void World::draw( Camera &camera )
{
	m_shader.bind();

	glm::mat4 view_matrix = camera.get_matrix();
	m_shader.set_mat4("view", glm::value_ptr(view_matrix));

	glm::vec3 cam_pos = camera.get_position(), cam_dir = camera.get_direction();


	for (auto& chunk : m_chunks) 
	{
		chunk.second->draw(camera, m_tilset, m_shader);
	}
}
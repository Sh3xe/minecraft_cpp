#include "world.hpp"

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <algorithm>

#include "core/timer.hpp"
#include "utils.hpp"
#include "core/logger.hpp"
#include "core/camera.hpp"
#include "blocks.hpp"

World::World( Config &config ):
	m_shader("resources/shaders/vertex.glsl", "", "resources/shaders/fragment.glsl"),
	m_tileset( "resources/images/" + config.texture_pack ),
	m_generator( m_db, this ),
	m_worker( &World::prepare_chunks, this )
{	
	m_render_distance = config.render_distance;

	// set shader uniforms
	glm::mat4 model_matrix(1.0f);
	glm::mat4 projection_matrix =
	glm::perspective (
		3.141592853f / 2.0f,
		config.window_width / static_cast<float>(config.window_height),
		0.01f, 256.0f
	);

	m_shader.bind();
	m_shader.set_mat4("projection", glm::value_ptr(projection_matrix));

	if( !m_db.load_blocks_from_file("resources/blocks/blocks.json"))
		SD_ERROR("Impossible de récuperer les infos sur les blocs");

	if( !m_db.load_structures_from_file("resources/blocks/structures.json"))
		SD_ERROR("Impossible de récuperer les infos sur les structures");
}

World::~World()
{
	m_running = false;
	if( m_worker.joinable() )
		m_worker.join();
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

	if( chunk != m_chunks.end() && y < CHUNK_HEIGHT && y >= 0 )
		chunk->second->set_block(coord_x, y, coord_z, type);
}

BlockID World::get_block(int x, int y, int z)
{
	auto [chunk_x, chunk_z] = get_pos_of_chunk(x, z);
	auto [coord_x, coord_z] = get_pos_inside_chunk(x, z);
	
	auto chunk = m_chunks.find(std::pair<int, int>(chunk_x * 16, chunk_z * 16));

	if( chunk != m_chunks.end() && y < CHUNK_HEIGHT && y >= 0 )
		return chunk->second->get_block(coord_x, y, coord_z);
	
	return 0;
}

void  World::prepare_chunks()
{
	using namespace std::chrono_literals;

	while( m_running )
	{
		if( m_update_queue.size() != 0 )
		{
			auto pos = m_update_queue.pop();
			
			m_map_mutex.lock();
			auto pair = m_chunks.find( pos );
			if( pair != m_chunks.end() )
			{
				auto &chunk = *(pair->second);
				if( chunk.state == ChunkState::need_generation )
				{
					m_generator.generate( chunk );
					add_blocks( chunk );
					chunk.state == ChunkState::need_mesh_update;
				}
				else if( chunk.state == ChunkState::need_mesh_update )
				{
					chunk.generate_mesh();
				}
			}
			m_map_mutex.unlock();
		}
		
		std::this_thread::sleep_for( 1ms );
	}
}

void World::update( double delta_time, Camera &camera ) 
{
	glm::ivec3 cam_pos = camera.get_position();
	auto [chunk_x, chunk_z] = get_pos_of_chunk( cam_pos.x, cam_pos.z );

	m_map_mutex.lock();
	
	// on supprime les tronçons trop éloigné
	for(auto chunk = m_chunks.begin(); chunk != m_chunks.end();) 
	{
		glm::ivec2 chunk_pos = chunk->second->get_position();
		if( // si le tronçon est trop loin
			chunk_pos.x > (chunk_x + m_render_distance) * 16 ||
			chunk_pos.x < (chunk_x - m_render_distance) * 16 ||
			chunk_pos.y > (chunk_z + m_render_distance) * 16 ||
			chunk_pos.y < (chunk_z - m_render_distance) * 16
			) 
		{
			chunk = m_chunks.erase(chunk);
		}
		else
		{
			if( chunk->second->state == ChunkState::ready )
				add_blocks( *chunk->second );
			
			if( chunk->second->state == ChunkState::need_mesh_update && !m_update_queue.in(chunk->first) )
				m_update_queue.push( chunk->first );

			++chunk;
		}
	}

	bool new_chunks {false};
	// on ajoute un tronçon si besoin
	for (int i = chunk_x - m_render_distance; i <= chunk_x + m_render_distance; ++i)
	for (int j = chunk_z - m_render_distance; j <= chunk_z + m_render_distance; ++j) 
	{
		auto it = m_chunks.find(std::pair<int, int>(i * 16, j * 16));
		if (it == m_chunks.end()) 
		{
			auto new_chunk = m_chunks.insert( std::pair< std::pair<int, int>, std::unique_ptr<Chunk>>(
				std::pair<int, int>(i * 16, j * 16), std::make_unique<Chunk>(m_db, i * 16, j * 16)
			)).first;

			new_chunks = true;
			new_chunk->second->state = ChunkState::need_generation;
			m_update_queue.push( new_chunk->first );
		}
	}

	if(new_chunks)
	{
		update_chunk_neighbours();
		m_update_queue.sort( [cam_pos]( const std::pair<int, int> &c1, const std::pair<int, int> &c2 )
		{
			glm::vec2 p1 = glm::vec2{ c1.first + (CHUNK_SIDE / 2), c1.second + (CHUNK_SIDE / 2)} - glm::vec2{cam_pos.x, cam_pos.z};
			glm::vec2 p2 = glm::vec2{ c2.first + (CHUNK_SIDE / 2), c2.second + (CHUNK_SIDE / 2)} - glm::vec2{cam_pos.x, cam_pos.z};

			return p1.x * p1.x + p1.y * p1.y < p2.x * p2.x + p2.y * p2.y ;
		});
	}

	m_map_mutex.unlock();
}

void World::add_blocks( Chunk &chunk )
{
	auto chunk_blocks = m_chunk_blocks.find( { chunk.m_position.x , chunk.m_position.y } );
	if( chunk_blocks == m_chunk_blocks.end() ) return;

	auto &blocks = chunk_blocks->second;

	if( !blocks.empty() )
		chunk.state = ChunkState::need_mesh_update;
	
	while( !blocks.empty() )
	{
		auto block = blocks.back();
		blocks.pop_back();
		if( block.block.second || chunk.fast_get(block.x, block.y, block.z) == 0 )
			chunk.fast_set( block.x, block.y, block.z, block.block.first );
		//if( block.block.first != 0 && block.y > chunk.m_layer_max )
		//	chunk.m_layer_max = block.y;
	}

	m_chunk_blocks.erase( chunk_blocks );
}

void World::draw( Camera &camera )
{
	m_shader.bind();

	glm::mat4 view_matrix = camera.get_matrix();
	m_shader.set_mat4("view", glm::value_ptr(view_matrix));
	auto cam_pos = camera.get_position();
	m_map_mutex.lock();

	// on tri les tronçons du plus éloigné au plus proche
	// par rapport à la camera et au centre des chunks
	std::vector<Chunk*> sorted_chunks;
	sorted_chunks.reserve( m_chunks.size() );
	for(auto &c: m_chunks) sorted_chunks.push_back( c.second.get() );

	std::sort( sorted_chunks.begin(), sorted_chunks.end(), [cam_pos]( Chunk *c1, Chunk *c2 )
	{
		glm::vec2 p1 = glm::vec2{ c1->get_position().x + (CHUNK_SIDE / 2), c1->get_position().y + (CHUNK_SIDE / 2)} - glm::vec2{cam_pos.x, cam_pos.z};
		glm::vec2 p2 = glm::vec2{ c2->get_position().x + (CHUNK_SIDE / 2), c2->get_position().y + (CHUNK_SIDE / 2)} - glm::vec2{cam_pos.x, cam_pos.z};

		return p1.x * p1.x + p1.y * p1.y > p2.x * p2.x + p2.y * p2.y ;
	} );

	// puis on les affiches
	for (auto &chunk: sorted_chunks)
	{
		if( chunk->state == ChunkState::need_upload )
		{
			for(int i = 0; i < 3; ++i)
				chunk->m_meshes[i].send_to_gpu();
			chunk->state = ChunkState::ready;
		}

		chunk->draw( camera, m_tileset, m_shader );

	}
	m_map_mutex.unlock();
}
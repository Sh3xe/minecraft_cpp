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

using namespace std;

World::World( const Settings &settings ):
	m_generator( this ),
	m_worker( &World::prepare_chunks, this )
{
	m_render_distance = settings.render_distance;
	m_generator.load_structs();
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
		auto neighbour = m_chunks.find( pair<int, int>(it->first.first - CHUNK_SIDE, it->first.second) );
		if (neighbour != m_chunks.end())
			it->second->neighbours[0] = neighbour->second.get();

		// same for +x
		neighbour = m_chunks.find( pair<int, int>(it->first.first + CHUNK_SIDE, it->first.second) );
		if(neighbour != m_chunks.end())
			it->second->neighbours[1] = neighbour->second.get();

		// same for -z
		neighbour = m_chunks.find( pair<int, int>(it->first.first, it->first.second - CHUNK_SIDE) );
		if(neighbour != m_chunks.end())
			it->second->neighbours[2] = neighbour->second.get();

		// same for +z
		neighbour = m_chunks.find( pair<int, int>(it->first.first, it->first.second + CHUNK_SIDE) );
		if(neighbour != m_chunks.end())	
			it->second->neighbours[3] = neighbour->second.get();
	}
}

void World::set_block(int x, int y, int z, blk::BlockType type)
{
	auto [chunk_x, chunk_z] = get_pos_of_chunk(x, z);
	auto [coord_x, coord_z] = get_pos_inside_chunk(x, z);

	auto chunk = m_chunks.find(pair<int, int>(chunk_x * CHUNK_SIDE, chunk_z * CHUNK_SIDE));

	if( chunk != m_chunks.end() && y < CHUNK_HEIGHT && y >= 0 )
		chunk->second->set_block(coord_x, y, coord_z, type);
}

blk::BlockType World::get_block(int x, int y, int z)
{
	auto [chunk_x, chunk_z] = get_pos_of_chunk(x, z);
	auto [coord_x, coord_z] = get_pos_inside_chunk(x, z);
	
	auto chunk = m_chunks.find(pair<int, int>(chunk_x * CHUNK_SIDE, chunk_z * CHUNK_SIDE));

	if( chunk != m_chunks.end() && y < CHUNK_HEIGHT && y >= 0 )
		return chunk->second->get_block(coord_x, y, coord_z);
	
	return blk::BlockType::air;
}

void World::prepare_chunks()
{
	using namespace chrono_literals;

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
					chunk.state = ChunkState::need_mesh_update;
				}
				else if( chunk.state == ChunkState::need_mesh_update )
				{
					chunk.construct_mesh();
					chunk.mesh.deb();
					chunk.state == ChunkState::need_upload;
				}
			}
			m_map_mutex.unlock();
		}
		
		this_thread::sleep_for( 1ms );
	}
}

void World::update( double delta_time, Camera &camera ) 
{
	auto [chunk_x, chunk_z] = get_pos_of_chunk( camera.position.x, camera.position.z );

	// delete far away chunks
	for(auto chunk = m_chunks.begin(); chunk != m_chunks.end();) 
	{
		//chunk->second->mesh.deb();
		m_map_mutex.lock();
		glm::ivec2 chunk_pos = chunk->second->position;
		if(
			chunk_pos.x > (chunk_x + m_render_distance) * CHUNK_SIDE ||
			chunk_pos.x < (chunk_x - m_render_distance) * CHUNK_SIDE ||
			chunk_pos.y > (chunk_z + m_render_distance) * CHUNK_SIDE ||
			chunk_pos.y < (chunk_z - m_render_distance) * CHUNK_SIDE
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
		m_map_mutex.unlock();
	}

	bool new_chunks {false};
	// add chunks if needed
	m_map_mutex.lock();
	for (int i = chunk_x - m_render_distance; i <= chunk_x + m_render_distance; ++i)
	for (int j = chunk_z - m_render_distance; j <= chunk_z + m_render_distance; ++j) 
	{
		auto it = m_chunks.find(pair<int, int>(i * CHUNK_SIDE, j * CHUNK_SIDE));
		if (it == m_chunks.end()) 
		{
			auto new_chunk = m_chunks.insert( pair< pair<int, int>, shared_ptr<Chunk>>(
				pair<int, int>(i * CHUNK_SIDE, j * CHUNK_SIDE), make_shared<Chunk>(i * CHUNK_SIDE, j * CHUNK_SIDE)
			)).first;

			new_chunks = true;
			new_chunk->second->state = ChunkState::need_generation;
			m_update_queue.push( new_chunk->first );
		}
	}

	if(new_chunks)
	{
		update_chunk_neighbours();
		/*
		m_update_queue.sort( [camera]( const pair<int, int> &c1, const pair<int, int> &c2 )
		{
			glm::vec2 p1 = glm::vec2{ c1.first + (CHUNK_SIDE / 2), c1.second + (CHUNK_SIDE / 2)} - glm::vec2{camera.position.x, camera.position.z};
			glm::vec2 p2 = glm::vec2{ c2.first + (CHUNK_SIDE / 2), c2.second + (CHUNK_SIDE / 2)} - glm::vec2{camera.position.x, camera.position.z};

			return p1.x * p1.x + p1.y * p1.y < p2.x * p2.x + p2.y * p2.y ;
		});*/
	}

	m_map_mutex.unlock();
}

void World::add_blocks( Chunk &chunk )
{
	auto chunk_blocks = m_chunk_blocks.find( { chunk.position.x , chunk.position.y } );
	if( chunk_blocks == m_chunk_blocks.end() ) return;

	auto &blocks = chunk_blocks->second;

	if( !blocks.empty() )
		chunk.state = ChunkState::need_mesh_update;
	
	while( !blocks.empty() )
	{
		auto block = blocks.back();
		blocks.pop_back();
		if( block.block.second || chunk.fast_get(block.x, block.y, block.z) == blk::BlockType::air )
			chunk.set_block( block.x, block.y, block.z, block.block.first );
		if( block.block.first != blk::BlockType::air )
			chunk.layers[block.y] = true;
	}

	m_chunk_blocks.erase( chunk_blocks );
}

/*
void World::render()
{
	m_map_mutex.lock();
	for (auto &[pos, c]: m_chunks)
	{
		if( c->state == ChunkState::need_upload )
		{
			for (int i = 0; i < 3; ++i)
			{
				upload(c->block_mesh);
				upload(c->foliage_mesh);
				upload(c->water_mesh);

			}
			c->state = ChunkState::ready;
		}

		renderer::submit(c);

	}
	m_map_mutex.unlock();
}*/
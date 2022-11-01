#pragma once

#include <cstdint>
#include <memory>

#include "utils.hpp"
#include "shader.hpp"
#include "texture2d.hpp"
#include "world/chunk.hpp"

struct Chunk;
class Camera;

class Renderer: public no_copy
{
public:
	Renderer() {}
	~Renderer() {}

	bool init();
	void shutdown();

	void set_clear_color( float r, float g, float b );
	void clear();

	void begin_scene( Camera &cam );
	void end_scene();

	void submit( std::shared_ptr<Chunk> chunk );

private:
	Texture2D m_tileset;
	std::vector< std::shared_ptr<Chunk> > m_chunks;

	glm::mat4 m_projection;
	glm::mat4 m_view;
};
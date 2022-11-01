#include "renderer.hpp"

#include <glad/glad.h>
#include <glm/ext.hpp>

#include "core/camera.hpp"
#include "world/chunk.hpp"
#include "graphics/texture2d.hpp"


bool Renderer::init()
{
	m_tileset.load_from_file("resources/images/tileset.png");
	m_projection = glm::perspective(3.141f/2.0f, 16.0f/9.0f, 0.01f, 100.0f);
	return false;
}

void Renderer::shutdown()
{

}

void Renderer::set_clear_color( float r, float g, float b )
{
	glClearColor(r, g, b, 1.0f);
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::begin_scene( Camera &cam )
{
	m_view = cam.get_view();
	m_chunks.clear();
}

void Renderer::end_scene()
{
	
}


void Renderer::submit( const std::shared_ptr<Chunk> &chunk )
{
	m_chunks.push_back(chunk);
}

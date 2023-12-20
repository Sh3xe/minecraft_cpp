#include "renderer.hpp"

#include "gl_functions.hpp"
#include <glm/ext.hpp>
#include <filesystem>

#include "core/camera.hpp"
#include "world/chunk.hpp"
#include "renderer/texture2d.hpp"

using namespace std::filesystem;

static Texture2D tileset;

static glm::mat4 projection;
static glm::mat4 view;

static std::vector< std::shared_ptr<ChunkData> > chunks;

namespace renderer
{
	void init()
	{
		tileset.load_from_file( "");
	}

	void set_clear_color( float r, float g, float b )
	{
		glClearColor(r, g, b, 1.0f);
	}
	
	void clear()
	{
		glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
	}

	void begin( const Camera &cam )
	{
		chunks.clear();
		view = cam.get_view();
		projection = cam.get_projection();
	}

	void end()
	{
		// sort chunks 
		// draw everything

		chunks.clear();
	}

	void submit(std::shared_ptr<ChunkData> chunk)
	{
		chunks.push_back(chunk);
	}
};
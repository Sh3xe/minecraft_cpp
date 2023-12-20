#pragma once

#include <cstdint>
#include <memory>

#include "shader.hpp"
#include "world/chunk.hpp"

struct ChunkData;
class Camera;

namespace renderer
{
	void init();

	void set_clear_color( float r, float g, float b );
	void clear();

	void begin( Camera &cam );
	void end();

	void submit( std::shared_ptr<ChunkData> chunk );
};
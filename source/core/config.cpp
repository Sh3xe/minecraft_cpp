#include "config.hpp"

#include <fstream>
#include <json.hpp>

#include "cmake_defines.hpp"
#include "core/logger.hpp"

using namespace nlohmann;

Config load_config_from_file( const std::string &path )
{
	std::fstream file { std::string{ROOT_DIR} + "/" + path };
	Config config
	{
		"Mincraft C++",
		1280,
		720,
		true, 60,
		0.05f,
		false
	};

	if( !file )
	{
		SD_WARN( "Impossible de charger: ", path );
		return config;
	}

	json data;
	file >> data;

	config.title = data["title"];
	config.window_width = data["window_width"];
	config.window_height = data["window_height"];
	config.fps_cap = data["fps_cap"];
	config.fps = data["fps"];
	config.sensitivity = data["sensitivity"];
	config.fullscreen = data["fullscreen"];
	config.render_distance = data["render_distance"];
	config.texture_pack = data["texture_pack"];

	file.close();

	return config;
}
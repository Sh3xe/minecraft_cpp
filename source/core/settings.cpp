#include "settings.hpp"

#include "core/config.hpp"
#include "core/logger.hpp"

std::optional<Settings> load_settings_from_file( const std::string &path )
{
	Config cfg { path };
	if( !cfg ) return {};

	Settings settings;
	try
	{
		settings.fps_cap = cfg.get_bool("fps_cap");
		settings.fullscreen = cfg.get_bool("fullscreen");

		settings.fps = cfg.get_int("fps");
		settings.width = cfg.get_int("window_width");
		settings.height = cfg.get_int("window_height");
		settings.render_distance = cfg.get_int("render_distance");
	
		settings.title = cfg.get_str("title");
		settings.texture_pack = cfg.get_str("texture_pack");
	
		settings.sensitivity = cfg.get_float("sensitivity");
	}
	catch(const std::exception& e)
	{
		SD_DEBUG( e.what() );
		SD_WARN(
			"L'un des attributs suivants:\n"
			"fps; fps_cap; sensitivity; window_width; window_height; render_distance; title; texture_pack; fullscreen\n"
			"n'est pas présent dans resources/settings.mcfg" );
		return {};
	}

	return settings;
}
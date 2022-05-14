#include "settings.hpp"

#include "core/config.hpp"

std::optional<Settings> load_settings_from_file( const std::string &path )
{
	Config cfg { path };
	if( !cfg ) return {};

	Settings settings;
	settings.fps = cfg.get_int("fps");
	settings.fps_cap = cfg.get_bool("fps_cap");
	settings.sensitivity = cfg.get_float("sensitivity");
	settings.width = cfg.get_int("window_width");
	settings.height = cfg.get_int("window_height");
	settings.render_distance = cfg.get_int("render_distance");
	settings.title = cfg.get_int("window_title");
	settings.texture_pack = cfg.get_int("texture_pack");
	settings.fullscreen = cfg.get_bool("fullscreen");

	return settings;
}
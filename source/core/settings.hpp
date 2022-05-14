#pragma once

#include <string>
#include <optional>

struct Settings
{
	int width = 1280;
	int height = 720;
	bool fps_cap = true;
	bool fullscreen = false;
	int fps = 30;
	float sensitivity = 0.05f;
	int render_distance = 6;
	std::string title = "Minecraft++";
	std::string texture_pack = "tileset2.png";
};

std::optional<Settings> load_settings_from_file( const std::string &path );
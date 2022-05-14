#include <SDL.h>
#include "gl_functions.hpp"

#include <filesystem>
#include "cmake_defines.hpp"

#include "game.hpp"
#include "core/logger.hpp"
#include "core/config.hpp"
#include "world/blocks.hpp"

SDL_GLContext context;
SDL_Window *window;

bool init_sdl( const Config &config)
{
	// init sdl
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS );

	// set gl atribs
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create window
	window = SDL_CreateWindow(
		config.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		config.window_width,
		config.window_height,
		SDL_WINDOW_OPENGL | ( config.fullscreen ? SDL_WINDOW_FULLSCREEN: 0 )
	);

	// hide cursor
	SDL_ShowCursor(0);
	SDL_SetRelativeMouseMode( SDL_TRUE );

	// create opengl context
	context = SDL_GL_CreateContext(window);

	glewInit();

	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, config.window_width, config.window_height );
	return true;
}

void destroy_sdl()
{
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
}

int main(int argc, char* argv[]) {
	
	// make cmake's "ROOT_DIR" the current path for the project
	std::filesystem::current_path( ROOT_DIR );

	Config config = load_config_from_file("resources/config/config.json");

	if( !init_sdl(config) )
	{
		SD_FATAL( "Impossible d'initialiser SDL2" );
		return 0;
	}

	if( !blk::load_structures("resources/terrain/structs.txt") )
	{
		SD_FATAL( "Impossible de charger les structures du jeu" );
		return 0;
	}

	Game game( config, window );
	game.run();

	return 0;
}
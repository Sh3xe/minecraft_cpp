#include <SDL.h>
#include <glad/glad.h>

#include <filesystem>
#include "cmake_defines.hpp"

#include "game/game.hpp"
#include "core/logger.hpp"
#include "core/config.hpp"
#include "world/blocks.hpp"

SDL_GLContext context;
SDL_Window *window;

bool init_sdl( const Settings &settings)
{
	// init sdl
	int res = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS );
	if(res != 0)
	{
		SD_DEBUG("impossible d'initialiser sdl:\n", SDL_GetError());
		return false;
	}

	// set gl atribs
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create window
	window = SDL_CreateWindow(
		settings.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		settings.width,
		settings.height,
		SDL_WINDOW_OPENGL | ( settings.fullscreen ? SDL_WINDOW_FULLSCREEN: 0 )
	);

	if( window == NULL )
	{
		SD_DEBUG("impossible d'ouvrir une fenetre");
		return false;
	}

	// hide cursor
	//SDL_ShowCursor(0);
	//SDL_SetRelativeMouseMode( SDL_TRUE );

	// create opengl context
	context = SDL_GL_CreateContext(window);

	if(!gladLoadGLLoader( (GLADloadproc)SDL_GL_GetProcAddress ))
	{
		SD_DEBUG("impossible de charger glad");
		return false;
	}

	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, settings.width, settings.height );
	return true;
}

void destroy_sdl()
{
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
}

int main(int argc, char* argv[])
{
	// make cmake's "ROOT_DIR" the current path for the project
	std::filesystem::current_path( ROOT_DIR );

	auto settings = load_settings_from_file( "resources/settings.mcfg" );
	if (!settings)
	{
		SD_WARN( "Impossible de récuperer les paramètres du jeu: chargement des paramètres par défaut");
		settings.emplace( Settings() ); // charge les paramètres par defaut
	}

	// initialisation openGL et SDL2
	if( !init_sdl(settings.value()) )
	{
		SD_FATAL( "Impossible d'initialiser SDL2" );
		return 0;
	}

	// chargement des resources 
	if( !blk::load_structures("resources/terrain/structs") )
	{
		SD_FATAL( "Impossible de charger les structures du jeu" );
		return 0;
	}

	// lancement du jeu
	Game game( settings.value(), window );
	game.run();

	return 0;
}
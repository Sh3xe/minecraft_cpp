#include <SDL.h>
#include "game.hpp"
#include "core/logger.hpp"

bool init_sdl()
{

	return true;
}

void destroy_sdl()
{

}

int main(int argc, char* argv[]) {
	
	if( !init_sdl() )
	{
		SD_FATAL( "Impossible d'initialiser SDL2" );
		return 0;
	}

	Config config = load_config_from_file( "resources/config/config.json" );

	Game game( config );
	game.run();

	return 0;
}
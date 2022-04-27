#include "game.hpp"

/*  TODO:
	- add better world generation
	- being able to place differents types of blocks
	- add collision
	- don't draw unseen chunks

	LONG TERM TODO:
	- add transparency and water when gud enough
	- add basic minecraft-like lighting
*/

int main(int argc, char* argv[]) {
	
	Config config ("resources/config/config.txt");

	Game game( config );
	game.run();

	return 0;
}
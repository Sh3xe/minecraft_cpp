#include "game.hpp"
#undef main // idk

/*  TODO:
	- fix ram consumption
	- add better world generation
	- fix chunk side mesh not updating correctly when generated

	LONG TERM TODO:
	- add transparency and water when gud enough
	- add basic minecraft-like lighting
	- être milliardaire
*/

int main(int argc, char* argv[]) {
	
	Config config;
	config.loadFromFile("resources/config/config.txt");

	Game game( config );
	game.run();

	return 0;
}
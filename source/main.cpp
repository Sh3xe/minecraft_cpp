#include "game.hpp"
#undef main // idk

int main(int argc, char* argv[]) {
	
	Config config;
	config.loadFromFile("resources/config/config.txt");

	Game game( config );
	game.run();

	return 0;
}
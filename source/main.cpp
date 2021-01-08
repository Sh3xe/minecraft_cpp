#include "game.hpp"

int main() {

	Config config {
		1280, 720,
		"Minecraft_cpp"
	};

	Game game( config );
	game.run();

	return 0;
}
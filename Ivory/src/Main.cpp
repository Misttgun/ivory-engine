#include "game/Game.h"

int main(int argc, char* argv[])
{
	Ivory::Game game;

	game.Init();
	game.Run();

	game.Destroy();

	return 0;
}

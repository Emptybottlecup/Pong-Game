#include "Game.h"
#include <iostream>

int main()
{
	Game* game = new Game(800,800);
	game->Run();
	delete game; 
}
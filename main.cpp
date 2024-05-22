#include "Game.h"
#include <iostream>
#include "GameStick.h"
#include "Ball.h"

int main()
{
	Game* game = new Game(800,800);
	std::vector<int> pIndices = { 0, 1, 2,2,1,3 };
	std::vector<DirectX::XMFLOAT4> pPoints =
	{
		DirectX::XMFLOAT4(0.95f, -0.80f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(1.0f, -0.80f, 0.5f,1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.95f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(1.0f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	TriangleGameComponent* OtherStick = new TriangleGameComponent(game, pPoints, pIndices);
	GameStick* Stick = new GameStick(game);
	Ball* ball = new Ball(game);
	game->PushGameComponents(Stick);
	game->PushGameComponents(OtherStick);
	game->PushGameComponents(ball);
	game->Run();
	delete game; 
}
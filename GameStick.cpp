/* #include "GameStick.h"
#include <iostream>

GameStick::GameStick(Game* game, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices) : TriangleGameComponent(game, points, indices) {}

DirectX::XMFLOAT4* GameStick::GetPoints()
{
	return pPoints.data();
}


void GameStick::UP_DOWN(Keys key, float deltaTime)
{
	if (key == Keys::S && pPoints[4].y > -1.0f)
	{
		pPoints[0].y += deltaTime * -speed;
		pPoints[2].y += deltaTime * -speed;
		pPoints[4].y += deltaTime * -speed;
		pPoints[6].y += deltaTime * -speed;
	}
	else if (key == Keys::W && pPoints[0].y < 1.0f)
	{
		pPoints[0].y += deltaTime * speed;
		pPoints[2].y += deltaTime * speed;
		pPoints[4].y += deltaTime * speed;
		pPoints[6].y += deltaTime * speed;
	}
}

GameStick::~GameStick()
{

} */

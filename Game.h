#pragma once
#include "DisplayWin32.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <wrl.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include "InputDevice.h"
#include "GameComponent.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class GameComponent;
class GameStick;
class Enemy;
class Ball;

class Game
{
public:

	Game();

	Game(int Width, int Height);

	void CreateDeviceAndSwapChain();

	void CreateRenderTargetView();

	void PushGameComponents(GameComponent* newGameComponent);

	ID3D11Device* GetDevice();

	ID3D11DeviceContext* GetDeviceContext();

	void Run();

	void Update(bool goal);

	void PushBalls(Ball* newBall);
	void DeleteResources();

	HWND* GetWindowHandle();
	~Game();

private:
	int pWidth = 800;
	int pHeight = 800;
	int PlayerScore = 0;
	int EnemyScore = 0;
	bool start = false;
	GameStick* pStick;
	std::vector<Ball*> pBalls;
	std::vector<Ball*> newBalls;
	Enemy* pEnemy;
	InputDevice* pInput;
	DisplayWin32 pWindow;
	std::vector<GameComponent*> pGameComponents;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
};
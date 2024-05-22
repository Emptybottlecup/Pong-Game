#include "Game.h"
#include <iostream>
#include <chrono>

Game::Game() : pWindow(pWidth, pHeight)
{
	pInput = new InputDevice(this);
	CreateDeviceAndSwapChain();
}

Game::Game(int Width, int Height) : pWidth(Width), pHeight(Height), pWindow(pWidth, pHeight)
{
	pInput = new InputDevice(this);
	CreateDeviceAndSwapChain();
}

void Game::CreateDeviceAndSwapChain()
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = static_cast<float>(pWidth);
	swapDesc.BufferDesc.Height = static_cast<float>(pHeight);
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 144;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = pWindow.GetWindow();
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = 0;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pDeviceContext);
}

void Game::PushGameComponents(GameComponent* newGameComponent)
{
	pGameComponents.push_back(newGameComponent);
}
void Game::CreateRenderTargetView()
{
	ID3D11Texture2D* colorBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&colorBuffer);
	pDevice->CreateRenderTargetView(colorBuffer, nullptr, &pRenderTargetView);
	if (colorBuffer)
	{
		colorBuffer->Release();
	}
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(pWidth);
	viewport.Height = static_cast<float>(pHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	pDeviceContext->RSSetViewports(1, &viewport);
}

ID3D11Device* Game::GetDevice()
{
	return pDevice;
}

ID3D11DeviceContext* Game::GetDeviceContext()
{
	return pDeviceContext;
}

void Game::Run()
{
	MSG msg = {};
	bool isExitRequested = false;
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();

	InputDevice::KeyboardInputEventArgs args; 
	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			args.VKey = msg.wParam;
			pInput->OnKeyDown(args);
			if (msg.message == WM_KEYDOWN) {
				args.Flags = msg.message;
				pInput->OnKeyDown(args);
			}
			else if (msg.message == WM_KEYUP) {
				args.Flags = msg.message;
				pInput->OnKeyDown(args);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			isExitRequested = true;
		}

		CreateRenderTargetView();

		float clearColor[] = { 0.0, 0.0, 0.0, 1 };
		pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);

		auto	curTime = std::chrono::steady_clock::now();
		float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 10000.0f;
		PrevTime = curTime;

		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
		for (auto object : pGameComponents)
		{
			pDeviceContext->VSSetShader(object->GetVertexShader().Get(), nullptr, 0);
			pDeviceContext->PSSetShader(object->GetPixelShader().Get(), nullptr, 0);

		}
		
		pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto object : pGameComponents)
		{
			object->Initialize();
			object->Draw();
		}
		
	    if (pInput->IsKeyDown(Keys::S))
		{
			pGameComponents[0]->UP_DOWN(Keys::S, deltaTime);
		}

		if (pInput->IsKeyDown(Keys::W))
		{
			pGameComponents[0]->UP_DOWN(Keys::W, deltaTime);
		}
		pGameComponents[2]->CheckPlayersCollisions(pGameComponents[0], pGameComponents[1]);
		pGameComponents[2]->Update(deltaTime,0);
		pGameComponents[1]->Update(deltaTime, pGameComponents[2]->GetPoints()->x);

		pSwapChain->Present(1, 0);
	}
}

void Game::DeleteResources()
{	pDeviceContext->Release();
	pDevice->Release();
	pSwapChain->Release();
	pRenderTargetView->Release();
	for (auto object : pGameComponents)
	{
		delete object;
	};
}

HWND* Game::GetWindowHandle() 
{
	return &(pWindow.GetWindow());
}

Game::~Game()
{
	DeleteResources();
}

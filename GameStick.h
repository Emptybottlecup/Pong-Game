#pragma once
#include "GameComponent.h"

class GameStick : public GameComponent
{
public:

	GameStick(Game* game);

	void Initialize();

	void CreateShadersAndInputLayout();

	void Draw();

	void Reload();

	void Update(float deltaTime, float y);

	DirectX::XMFLOAT4* GetPoints();

	void CheckPlayersCollisions(GameComponent* player_1, GameComponent* player_2);
	void UP_DOWN(Keys key, float velocity);

	void DestroyResources();

	~GameStick();

	Microsoft::WRL::ComPtr<ID3D11VertexShader>& GetVertexShader()
	{
		return pVertexShader;
	}

	Microsoft::WRL::ComPtr<ID3D11PixelShader>& GetPixelShader()
	{
		return pPixelShader;
	}

	Microsoft::WRL::ComPtr<ID3DBlob>& GetVertexShaderByteCode()
	{
		return pVertexShaderByteCode;
	}

	Microsoft::WRL::ComPtr<ID3DBlob>& GetPixelShaderByteCode()
	{
		return pPixelShaderByteCode;
	}

private:
	std::vector<int> pIndices = { 0, 1, 2,2,1,3 };
	std::vector<DirectX::XMFLOAT4> pPoints =
	{
		DirectX::XMFLOAT4(-1.0f, -0.80f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.95f, -0.80f, 0.5f,1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-1.0f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.95f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	float speed = 0.015f;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pPixelShaderByteCode;
	ID3D11InputLayout* pInputLayout;
	Game* pGame;
};

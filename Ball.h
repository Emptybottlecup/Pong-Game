#pragma once
#include "GameComponent.h"

class Ball : public GameComponent
{
public:
	Ball (Game* game);

	void Initialize();

	void CreateShadersAndInputLayout();

	void Draw();

	void Reload();

	void Update(float deltaTime, float y);

	void ChangeDirection(float deltatime);

	DirectX::XMFLOAT4* GetPoints();

	void CheckPlayersCollisions(GameComponent* player_1, GameComponent* player_2);

	void CheckCollisions(float pos_1_x, float pos_2_x, float pos_3_x, float pos_4_x, float pos_1_y, float pos_2_y, float pos_3_y, float pos_4_y);

	void UP_DOWN(Keys key, float velocity);

	void DestroyResources();

	~Ball ();

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
	std::vector<int> pIndices = { 0, 1, 2,2, 1,3};
	DirectX::XMFLOAT2 direction = {1.0f, 0.0f };
	std::vector<DirectX::XMFLOAT4> pPoints =
	{
		DirectX::XMFLOAT4(-0.02f, 0.02f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.02f, 0.02f, 0.5f,1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.02f, -0.02f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.02f, -0.02f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pPixelShaderByteCode;
	ID3D11InputLayout* pInputLayout;
	float speed = 0.015f;
	DirectX::XMFLOAT4* Player_1_Points;
	DirectX::XMFLOAT4* Player_2_Points;
	Game* pGame;
};
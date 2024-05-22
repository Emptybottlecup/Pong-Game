#pragma once
#include "Keys.h"
#include "Game.h"

class Game;

class GameComponent
{
public:
	virtual void Initialize() = 0;
	virtual void Draw() = 0;
	virtual void CreateShadersAndInputLayout() = 0;
	virtual void DestroyResources() = 0;
	virtual void Reload() = 0;
	virtual void Update(float deltaTime, float y) = 0;

	virtual void CheckPlayersCollisions(GameComponent* player_1, GameComponent* player_2) = 0;

	virtual DirectX::XMFLOAT4* GetPoints() = 0;
	virtual void UP_DOWN(Keys key, float velocity) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11VertexShader>& GetVertexShader() = 0;

	virtual Microsoft::WRL::ComPtr<ID3D11PixelShader>& GetPixelShader() = 0;

	virtual Microsoft::WRL::ComPtr<ID3DBlob>& GetVertexShaderByteCode() = 0;

	virtual Microsoft::WRL::ComPtr<ID3DBlob>& GetPixelShaderByteCode() = 0;
};

class TriangleGameComponent : public GameComponent
{
public:

	TriangleGameComponent(Game* GameObject, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices);

	void Initialize();

	void CreateShadersAndInputLayout();

	void Draw();

	void UP_DOWN(Keys key, float velocity);
	void Reload();

	void CheckPlayersCollisions(GameComponent* player_1, GameComponent* player_2);
	void Update(float deltaTime, float y);

	DirectX::XMFLOAT4* GetPoints();
	void DestroyResources();

	~TriangleGameComponent();

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
	bool down_up = true;
	std::vector<DirectX::XMFLOAT4> pPoints;
	std::vector<int> pIndices;
	float speed = 0.015f;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pPixelShaderByteCode;
	ID3D11InputLayout* pInputLayout;
	Game* pGame;
};
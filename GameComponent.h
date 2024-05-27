#pragma once
#include "Keys.h"
#include "Game.h"

class Game;
class Enemy;
class Ball;

class GameComponent
{
public:
	virtual void Initialize() = 0;
	virtual void Draw() = 0;
	virtual void CreateShadersAndInputLayout() = 0;
	virtual void DestroyResources() = 0;
	virtual void Reload() = 0;

	virtual void Update(DirectX::XMFLOAT3 position) = 0;
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

	void Reload();

	void Update(DirectX::XMFLOAT3 position);

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
protected:
	std::vector<DirectX::XMFLOAT4> pPoints;
	std::vector<int> pIndices;
	Game* pGame;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pPixelShaderByteCode;
	ID3D11InputLayout* pInputLayout;
};



class GameStick
{
public:

	GameStick(Game* game, float pos_x, float pos_y, float width, float height);
	void UP_DOWN(Keys key, float deltaTime);

	TriangleGameComponent* GetStick();

	float GetXPosition();

	float GetYPosition();

	float GetWidth();

	float GetHeight();
	~GameStick();

private:
	TriangleGameComponent* pStick = nullptr;
	float pPosition_x = 0.0f;
	float pPosition_y = 0.0f;
	float pWidth = 0.0f;
	float pHeight = 0.0f;
	float pSpeed = 0.01f;
};

/**
class Ball : public TriangleGameComponent
{
public:

	Ball(Game* game, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices);


	void ChangePoints(float deltaTime, GameStick* player, Enemy* enemy);

	void Reset();
	float GetYVector();

	~Ball();

private:
	void CheckPlayersCollisions(GameStick* player, Enemy* enemy, float deltaTime);
	void CheckCollisions(float pos_1_x, float pos_2_x, float pos_3_x, float pos_4_x, float pos_1_y, float pos_2_y, float pos_3_y, float pos_4_y, GameStick* player, Enemy* enemy);
	DirectX::XMFLOAT2 direction = { 1.0f, 0.0f };
	float speed = 0.01f;
};



class Enemy : public TriangleGameComponent
{
public:
	Enemy(Game* GameObject, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices);
	void ChangePoints(float deltaTime, float last_ball_location);

	void Reset();
	DirectX::XMFLOAT4* GetPoints();
	~Enemy();
private:
	float speed = 0.01;
};
*/
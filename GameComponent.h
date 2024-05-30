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
	virtual void Reload(DirectX::XMFLOAT4 position) = 0;

	virtual void Update() = 0;
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

	void Reload(DirectX::XMFLOAT4 position);

	void Update();

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

	Game* GetGame() 
	{
		return pGame;
	}
protected:
	std::vector<DirectX::XMFLOAT4> pPoints;
	std::vector<int> pIndices;
	ID3D11Buffer* ConstantBuff;
	DirectX::XMFLOAT4 pPosition = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
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

	void Reset();

	float GetHeight();
	~GameStick();

private:
	TriangleGameComponent* pStick = nullptr;
	float pPosition_x = 0.0f;
	float pPosition_y = 0.0f;
	float pWidth = 0.0f;
	float pHeight = 0.0f;
	float pSpeed = 0.2f;
};


class Ball 
{
public:

	Ball(Game* game, float pos_x, float pos_y, float width, float height);


	void Update(float updateTime, GameStick* player, Enemy* enemy);

	TriangleGameComponent* GetBall();

	float GetXPosition();

	float GetYPosition();

	bool CheckCollisionsPlayer(GameStick* player, float deltaTime);

	bool CheckCollisionsEnemy(Enemy* player, float deltaTime);
	float GetWidth();

	float GetHeight();

	void Reset();

	~Ball();

private:
	TriangleGameComponent* pBall = nullptr;
	float pPosition_x = 0.0f;
	float pPosition_y = 0.0f;
	float pWidth = 0.0f;
	float pHeight = 0.0f;
	float direction_y = 0;
	float direction_x = 1;
 	float speed = 0.2;
};

class Enemy 
{
public:
	Enemy (Game* game, float pos_x, float pos_y, float width, float height);

	void Update(float updateTime, Ball*);

	TriangleGameComponent* GetEnemy();

	float GetXPosition();

	float GetYPosition();

	float GetWidth();

	void Reset();
	float GetHeight();

	~Enemy();
private:
	TriangleGameComponent* pEnemy = nullptr;
	float pPosition_x = 0.0f;
	float pPosition_y = 0.0f;
	float pWidth = 0.0f;
	float pHeight = 0.0f;
	float pSpeed = 0.2f;
};

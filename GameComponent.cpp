#include "GameComponent.h"
#include <iostream>

struct ConstantBuffer
{
	DirectX::XMFLOAT4 offset;
};

TriangleGameComponent::TriangleGameComponent(Game* GameObject, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices) : pGame(GameObject), pPoints(points), pIndices(indices)
{
	CreateShadersAndInputLayout();
}

void TriangleGameComponent::Initialize()
{
	D3D11_BUFFER_DESC vertexBufDesc;
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(pPoints);
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pPoints.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	pGame->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &VertexBuffer);

	D3D11_BUFFER_DESC indexBufDesc;
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(pIndices);
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	auto hr = pGame->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &IndexBuffer);

	ConstantBuffer cb = { DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };
	D3D11_BUFFER_DESC constantBufDesc;
	constantBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufDesc.MiscFlags = 0;
	constantBufDesc.StructureByteStride = 0;
	constantBufDesc.ByteWidth = sizeof(ConstantBuffer);
	D3D11_SUBRESOURCE_DATA constantData;
	constantData.pSysMem = &cb;
	constantData.SysMemPitch = 0;
	constantData.SysMemSlicePitch = 0;

	hr = pGame->GetDevice()->CreateBuffer(&constantBufDesc, &constantData, &ConstantBuff);
	pGame->GetDeviceContext()->IASetInputLayout(pInputLayout);
}

void TriangleGameComponent::CreateShadersAndInputLayout()
{
	ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"Shaders.txt",
		nullptr,
		nullptr,
		"VSMAIN",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
		0,
		&pVertexShaderByteCode,
		&errorVertexCode);

	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(L"Shaders.txt", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pPixelShaderByteCode, &errorPixelCode);

 	if (FAILED(res)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(*(pGame->GetWindowHandle()), L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}
	}

	pGame->GetDevice()->CreateVertexShader(
		pVertexShaderByteCode->GetBufferPointer(),
		pVertexShaderByteCode->GetBufferSize(),
		nullptr, &pVertexShader);

	pGame->GetDevice()->CreatePixelShader(
		pPixelShaderByteCode->GetBufferPointer(),
		pPixelShaderByteCode->GetBufferSize(),
		nullptr, &pPixelShader);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0},

		D3D11_INPUT_ELEMENT_DESC {
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		16,
		D3D11_INPUT_PER_VERTEX_DATA,
		0}
	};

	pGame->GetDevice()->CreateInputLayout(inputElements, 2, pVertexShaderByteCode->GetBufferPointer(), pVertexShaderByteCode->GetBufferSize(), &pInputLayout);
}

void TriangleGameComponent::Draw()
{
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	pGame->GetDeviceContext()->VSSetConstantBuffers(0, 1, &ConstantBuff);
	pGame->GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, strides, offsets);
	pGame->GetDeviceContext()->DrawIndexed(pIndices.size(), 0, 0);
}

void TriangleGameComponent::Reload(DirectX::XMFLOAT4 position)
{
	pPosition = position;
}

void TriangleGameComponent::Update()
{
	ConstantBuffer new_pos = { pPosition };
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pGame->GetDeviceContext()->Map(ConstantBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &new_pos, sizeof(ConstantBuffer));
	pGame->GetDeviceContext()->Unmap(ConstantBuff, 0);
}

void TriangleGameComponent::DestroyResources()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pVertexShaderByteCode->Release();
	pPixelShaderByteCode->Release();
	pInputLayout->Release();
}

TriangleGameComponent::~TriangleGameComponent()
{
	pGame = nullptr;
	DestroyResources();
}


GameStick::GameStick(Game* game, float pos_x, float pos_y, float width, float height) : pPosition_x(pos_x), pPosition_y(pos_y), pWidth(width), pHeight(height)
{
	std::vector<DirectX::XMFLOAT4> points =
	{
		DirectX::XMFLOAT4(pos_x - width / 2, pos_y + height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x + width / 2, pos_y + height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x - width / 2, pos_y - height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x + width / 2, pos_y - height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};

	std::vector <int> indices = { 0,1,2,2,1,3 };
	pStick = new TriangleGameComponent(game, points, indices);
}

void GameStick::UP_DOWN(Keys key, float deltaTime)
{
	if (key == Keys::S && (pPosition_y - pHeight / 2) - pSpeed * deltaTime >= -1.0f)
	{
		pPosition_y -= pSpeed * deltaTime;
	}
	else if (key == Keys::W && (pPosition_y + pHeight / 2) + pSpeed * deltaTime <= 1.0f)
	{
		pPosition_y += pSpeed * deltaTime;
	}
	else if (key == Keys::Down && (pPosition_y - pHeight / 2) - pSpeed * deltaTime >= -1.0f)
	{
		pPosition_y -= pSpeed * deltaTime;
	}
	else if (key == Keys::Up && (pPosition_y + pHeight / 2) + pSpeed * deltaTime <= 1.0f)
	{
		pPosition_y += pSpeed * deltaTime;
	}
	pStick->Reload(DirectX::XMFLOAT4(0.0f, pPosition_y, 0.0f, 0.0f));
}

TriangleGameComponent* GameStick::GetStick()
{
	return pStick;
}

float GameStick::GetXPosition()
{
	return pPosition_x;
}

float GameStick::GetYPosition()
{
	return pPosition_y;
}

float GameStick::GetWidth()
{
	return pWidth;
}

void GameStick::Reset()
{
	pPosition_y = 0.0f;
	pStick->Reload(DirectX::XMFLOAT4(0, 0, 0.0f, 0.0f));
}

float GameStick::GetHeight()
{
	return pHeight;
}
GameStick::~GameStick()
{

}

Ball::Ball(Game* game, float pos_x, float pos_y, float width, float height , float direcxtion_x, float direction_y) : pPosition_x(pos_x), pPosition_y(pos_y), pWidth(width), pHeight(height), direction_x(direcxtion_x), direction_y(direction_y)
{
	std::vector<DirectX::XMFLOAT4> points =
	{
		DirectX::XMFLOAT4(pos_x - width / 2, pos_y + height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x + width / 2, pos_y + height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x - width / 2, pos_y - height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x + width / 2, pos_y - height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};

	std::vector <int> indices = { 0,1,2,2,1,3 };
	pBall = new TriangleGameComponent(game, points, indices);
}

void Ball::Update(float deltaTime, GameStick* player, GameStick* enemy)
{
	if (CheckCollisionsPlayer(player, deltaTime))
	{
		speed += 0.0001f;
		float angle = ((pPosition_y + speed * deltaTime * direction_y - player->GetYPosition()) / player->GetHeight() / 2);
		direction_y =  angle * 2;
		direction_x = -direction_x;
		wasCollided = true;
	}

	else if (CheckCollisionsPlayer(enemy, deltaTime))
	{
		speed += 0.0001f;
		float angle = ((pPosition_y + speed * deltaTime * direction_y - enemy->GetYPosition()) / enemy->GetHeight() / 2);
		direction_y = angle * 2;
		direction_x = -direction_x;
		wasCollided = true;
	} 
	else if ((pPosition_x + deltaTime * speed * direction_x) - pWidth / 2 <= -1.0f)
	{
		direction_x = -direction_x;
	}
	else if ((pPosition_x + deltaTime * speed * direction_x) + pWidth / 2 >= 1.0f)
	{
		direction_x = -direction_x;
	}
	else if ((pPosition_y + deltaTime * speed * direction_y) - pHeight / 2 <= -1.0f)
	{
		direction_y = -direction_y;
	}
	else if ((pPosition_y + deltaTime * speed * direction_y) + pHeight / 2 >= 1.0f)
	{
		direction_y = -direction_y;
	}
	pPosition_x += deltaTime * speed * direction_x;
	pPosition_y += deltaTime * speed * direction_y;
	pBall->Reload(DirectX::XMFLOAT4(pPosition_x, pPosition_y, 0.0f, 0.0f));
}

bool Ball::CheckCollisionsPlayer(GameStick* player, float deltaTime)
{
	float minX1 = (pPosition_x + speed * deltaTime * direction_x) - pWidth / 2;
	float maxX1 = (pPosition_x + speed * deltaTime * direction_x) + pWidth / 2;
	float minY1 = (pPosition_y + speed * deltaTime * direction_y) - pHeight / 2;
	float maxY1 = (pPosition_y + speed * deltaTime * direction_y) + pHeight / 2;

	float minX2 = player->GetXPosition() - player->GetWidth() / 2;
	float maxX2 = player->GetXPosition() + player->GetWidth() / 2;
	float minY2 = player->GetYPosition() - player->GetHeight() / 2;
	float maxY2 = player->GetYPosition() + player->GetHeight() / 2;
	return !(maxX1 < minX2 || minX1 > maxX2 || maxY1 < minY2 || minY1 > maxY2);
}


bool Ball::CheckCollisionsEnemy(Enemy* player, float deltaTime)
{
	float minX1 = (pPosition_x + speed * deltaTime * direction_x) - pWidth / 2;
	float maxX1 = (pPosition_x + speed * deltaTime * direction_x) + pWidth / 2;
	float minY1 = (pPosition_y + speed * deltaTime * direction_y) - pHeight / 2;
	float maxY1 = (pPosition_y + speed * deltaTime * direction_y) + pHeight / 2;

	float minX2 = player->GetXPosition() - player->GetWidth() / 2;
	float maxX2 = player->GetXPosition() + player->GetWidth() / 2;
	float minY2 = player->GetYPosition() - player->GetHeight() / 2;
	float maxY2 = player->GetYPosition() + player->GetHeight() / 2;

	return !(maxX1 < minX2 || minX1 > maxX2 || maxY1 < minY2 || minY1 > maxY2);
}

TriangleGameComponent* Ball::GetBall()
{
	return pBall;
}

float Ball::GetXPosition()
{
	return pPosition_x;
}

float Ball::GetXDirection()
{
	return direction_x;
}

float Ball::GetYDirection()
{
	return direction_y;
}

void Ball::SetXPosition(float pos_x)
{
	pPosition_x = pos_x;
}

void Ball::SetYPosition(float pos_y)
{
	pPosition_y = pos_y;
}

float Ball::GetYPosition()
{
	return pPosition_y;
}

float Ball::GetWidth()
{
	return pWidth;
}

void Ball::SetCollideFalse()
{
	wasCollided = false;
}

float Ball::GetHeight()
{
	return pHeight;
}

bool Ball::WasCollided()
{
	return wasCollided;
}

void Ball::Reset()
{
	pPosition_x = 0.0f;
	pPosition_y = 0.0f;
	float direction_y = 0;
	float direction_x = 1;
	speed = 0.2f;
	pBall->Reload(DirectX::XMFLOAT4(0, 0, 0.0f, 0.0f));
}

Ball::~Ball()
{
}

Enemy::Enemy(Game* game, float pos_x, float pos_y, float width, float height) : pPosition_x(pos_x), pPosition_y(pos_y), pWidth(width), pHeight(height)
{
	std::vector<DirectX::XMFLOAT4> points =
	{
		DirectX::XMFLOAT4(pos_x - width / 2, pos_y + height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x + width / 2, pos_y + height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x - width / 2, pos_y - height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(pos_x + width / 2, pos_y - height / 2, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};

	std::vector <int> indices = { 0,1,2,2,1,3 };
	pEnemy = new TriangleGameComponent(game, points, indices);
}

void Enemy::Update(float updateTime, Ball* ball)
{
	if (pPosition_y < ball->GetYPosition() && pPosition_y + pHeight / 2 < 1.0f)
	{
		pPosition_y += updateTime * pSpeed;
	}
	else if(pPosition_y > ball->GetYPosition() && pPosition_y - pHeight / 2 > -1.0f)
	{
		pPosition_y -= updateTime * pSpeed;
	}
	pEnemy->Reload(DirectX::XMFLOAT4(0, pPosition_y, 0.0f, 0.0f));
}

TriangleGameComponent* Enemy::GetEnemy()
{
	return pEnemy;
}

float Enemy::GetXPosition()
{
	return pPosition_x;
}

float Enemy::GetYPosition()
{
	return pPosition_y;
}

float Enemy::GetWidth()
{
	return pWidth;
}

void Enemy::Reset()
{
	pPosition_y = 0.0f;
	pEnemy->Reload(DirectX::XMFLOAT4(0, 0, 0.0f, 0.0f));
}

float Enemy::GetHeight()
{
	return pHeight;
}

Enemy::~Enemy()
{

}

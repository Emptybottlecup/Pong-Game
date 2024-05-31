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

/*
Ball::Ball(Game* game, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices) : TriangleGameComponent(game, points, indices)
{
}

void Ball::ChangePoints(float deltaTime, GameStick* player, Enemy* enemy)
{
	CheckPlayersCollisions(player, enemy, deltaTime);
	auto new_position = DirectX::XMFLOAT3(direction.x * speed * deltaTime, direction.y * speed * deltaTime, 0.0f);
	pGame->GetDeviceContext()->UpdateSubresource(pConstantBuffer, 0, nullptr, &new_position, 0, 0);
	pGame->GetDeviceContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
}

float Ball::GetYVector()
{
	return (pPoints[2].y + pPoints[6].y) / 2;
}

void Ball::Reset() 
{
	pPoints[0].x = -0.02f;
	pPoints[2].x = 0.02f;
	pPoints[4].x = -0.02f;
	pPoints[6].x = 0.02f;
	pPoints[0].y = 0.02f;
	pPoints[2].y = 0.02f;
	pPoints[4].y = -0.02f;
	pPoints[6].y = -0.02f;
	direction = { 1, 0 };
	speed = 0.01f;
}

void Ball::CheckPlayersCollisions(GameStick* player, Enemy* enemy, float deltaTime)
{
	float new_pos_1_x = pPoints[0].x + direction.x * speed * deltaTime;
	float new_pos_2_x = pPoints[2].x + direction.x * speed * deltaTime;
	float new_pos_3_x = pPoints[4].x + direction.x * speed * deltaTime;
	float new_pos_4_x = pPoints[6].x + direction.x * speed * deltaTime;
	float new_pos_1_y = pPoints[0].y + direction.y * speed * deltaTime;
	float new_pos_2_y = pPoints[2].y + direction.y * speed * deltaTime;
	float new_pos_3_y = pPoints[4].y + direction.y * speed * deltaTime;
	float new_pos_4_y = pPoints[6].y + direction.y * speed * deltaTime;
	CheckCollisions(new_pos_1_x, new_pos_2_x, new_pos_3_x, new_pos_4_x, new_pos_1_y, new_pos_2_y, new_pos_3_y, new_pos_4_y, player, enemy);
}

void Ball::CheckCollisions(float pos_1_x, float pos_2_x, float pos_3_x, float pos_4_x, float pos_1_y, float pos_2_y, float pos_3_y, float pos_4_y, GameStick* player, Enemy* enemy)
{

	if (pos_1_x <= -1.0f || pos_3_x <= -1.0f)
	{
		pGame->Update(false);
	}
	else if (pos_2_x >= 1.0f || pos_4_x >= 1.0f)
	{
		pGame->Update(true);
	}
	else if (pos_1_y >= 1.0f || pos_2_y >= 1.0f)
	{
		direction.y = -direction.y;
	}
	else if (pos_3_y <= -1.0f || pos_4_y <= -1.0f)
	{
		direction.y = -direction.y;
	}

	if (((player->GetPoints()[6].y <= pos_1_y && pos_1_y <= player->GetPoints()[2].y) || (player->GetPoints()[6].y <= pos_3_y && pos_3_y <= player->GetPoints()[2].y)) && player->GetPoints()[6].x >= pos_1_x)
	{
		speed += 0.0001f;
		auto middle_position_ball_left_y = (pos_1_y + pos_3_y) / 2;
		auto middle_position_player_left_y = (player->GetPoints()[2].y + player->GetPoints()[6].y) / 2;
		float angle = (middle_position_ball_left_y - middle_position_player_left_y) / (fabs(player->GetPoints()[6].y - player->GetPoints()[2].y) / 2);
		direction.x = -direction.x;
		direction.y += angle;
	}

	else if (((enemy->GetPoints()[4].y <= pos_2_y && pos_2_y <= enemy->GetPoints()[0].y) || (enemy->GetPoints()[4].y <= pos_4_y && pos_4_y <= enemy->GetPoints()[0].y)) && enemy->GetPoints()[4].x <= pos_2_x)
	{
		speed += 0.0001f;
		auto middle_position_ball_right_y = (pos_2_y + pos_4_y) / 2;
		auto middle_position_player_right_y = (enemy->GetPoints()[0].y + enemy->GetPoints()[4].y) / 2;
		float angle = (middle_position_ball_right_y - middle_position_player_right_y) / (fabs(enemy->GetPoints()[4].y - enemy->GetPoints()[0].y) / 2);
		direction.x = -direction.x;
		direction.y += angle;
	}
}

Ball::~Ball()
{
}

Enemy::Enemy(Game* GameObject, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indices) : TriangleGameComponent(GameObject, points, indices) {}

void Enemy::ChangePoints(float deltaTime, float last_ball_location)
{
	if ((pPoints[0].y + pPoints[4].y) / 2 < last_ball_location && pPoints[0].y <= 1.0f)
	{
		pPoints[0].y += deltaTime * speed;
		pPoints[2].y += deltaTime * speed;
		pPoints[4].y += deltaTime * speed;
		pPoints[6].y += deltaTime * speed;
	}

	else if ((pPoints[0].y + pPoints[4].y) / 2 > last_ball_location && pPoints[0].y >= -1.0f)
	{
		pPoints[0].y -= deltaTime * speed;
		pPoints[2].y -= deltaTime * speed;
		pPoints[4].y -= deltaTime * speed;
		pPoints[6].y -= deltaTime * speed;
	}
}


void Enemy::Reset()
{
	pPoints[0].x = 0.98f;
	pPoints[2].x = 1.0f;
	pPoints[4].x = 0.98f;
	pPoints[6].x = 1.0f;
	pPoints[0].y = 0.1f;
	pPoints[2].y = 0.1f;
	pPoints[4].y = -0.1f;
	pPoints[6].y = -0.1f;
}

DirectX::XMFLOAT4* Enemy::GetPoints()
{
	return pPoints.data();
}
Enemy::~Enemy()
{
}
*/

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

void Ball::Update(float deltaTime, GameStick* player, Enemy* enemy)
{
	if (CheckCollisionsPlayer(player, deltaTime))
	{
		speed += 0.0001f;
		float angle = ((pPosition_y + speed * deltaTime * direction_y - player->GetYPosition()) / player->GetHeight() / 2);
		direction_y =  angle * 2;
		direction_x = -direction_x;
		wasCollided = true;
	}

	else if (CheckCollisionsEnemy(enemy, deltaTime))
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

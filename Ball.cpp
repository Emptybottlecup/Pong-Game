#include "Ball.h"
#include <iostream>
Ball::Ball(Game* game) : pGame(game)
{
	CreateShadersAndInputLayout();
}

void Ball::Initialize()
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
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
	pGame->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &IndexBuffer);

	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };

	pGame->GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pGame->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, strides, offsets);
	pGame->GetDeviceContext()->IASetInputLayout(pInputLayout);
}

void Ball::CreateShadersAndInputLayout()
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

void Ball::Draw()
{
	pGame->GetDeviceContext()->DrawIndexed(pIndices.size(), 0, 0);
}

void Ball::Reload()
{
}

void Ball::Update(float deltaTime, float y)
{
	ChangeDirection(deltaTime);
	pPoints[0].x += direction.x * speed * deltaTime;
	pPoints[2].x += direction.x * speed * deltaTime;
	pPoints[4].x += direction.x * speed * deltaTime;
	pPoints[6].x += direction.x * speed * deltaTime;
	pPoints[0].y += direction.y * speed * deltaTime;
	pPoints[2].y += direction.y * speed * deltaTime;
	pPoints[4].y += direction.y * speed * deltaTime;
	pPoints[6].y += direction.y * speed * deltaTime;
}

void Ball::ChangeDirection(float deltaTime)
{
	float new_pos_1_x = pPoints[0].x + direction.x * speed * deltaTime;
	float new_pos_2_x = pPoints[2].x + direction.x * speed * deltaTime;
	float new_pos_3_x = pPoints[4].x + direction.x * speed * deltaTime;
	float new_pos_4_x = pPoints[6].x + direction.x * speed * deltaTime;
	float new_pos_1_y = pPoints[0].y + direction.y * speed * deltaTime;
	float new_pos_2_y = pPoints[2].y + direction.y * speed * deltaTime;
	float new_pos_3_y = pPoints[4].y + direction.y * speed * deltaTime;
	float new_pos_4_y = pPoints[6].y + direction.y * speed * deltaTime;
	
	CheckCollisions(new_pos_1_x, new_pos_2_x, new_pos_3_x, new_pos_4_x, new_pos_1_y, new_pos_2_y, new_pos_3_y, new_pos_4_y);
}

DirectX::XMFLOAT4* Ball::GetPoints()
{
	float y = (pPoints[2].y + pPoints[6].y) / 2;
	return new DirectX::XMFLOAT4(y, 0, 0, 0);
}

void Ball::CheckPlayersCollisions(GameComponent* player_1, GameComponent* player_2)
{
	Player_1_Points = player_1->GetPoints();
	Player_2_Points = player_2->GetPoints();
}

void Ball::CheckCollisions(float pos_1_x, float pos_2_x, float pos_3_x, float pos_4_x, float pos_1_y, float pos_2_y, float pos_3_y, float pos_4_y)
{

	if (pos_1_x <= -1.0f || pos_3_x <= -1.0f)
	{
		direction.x = -direction.x;
	}
	else if (pos_2_x >= 1.0f || pos_4_x >= 1.0f)
	{
		direction.x = -direction.x;
	}
	else if (pos_1_y >= 1.0f || pos_2_y >= 1.0f)
	{
		direction.y = -direction.y;
	}
	else if (pos_3_y <= -1.0f || pos_4_y <= -1.0f)
	{
		direction.y = -direction.y;
	}

	if (((Player_1_Points[6].y <= pos_1_y && pos_1_y <= Player_1_Points[2].y) || (Player_1_Points[6].y <= pos_3_y && pos_3_y <= Player_1_Points[2].y)) && Player_1_Points[6].x >= pos_1_x)
	{
		auto middle_position_ball_left_y = (pos_1_y + pos_3_y) / 2;
		auto middle_position_player_left_y = (Player_1_Points[2].y + Player_1_Points[6].y) / 2;
		float angle = (middle_position_ball_left_y - middle_position_player_left_y) / (fabs(Player_1_Points[6].y - Player_1_Points[2].y) / 2);
		float length = sqrt(direction.x * direction.x + angle * angle);
		direction.x = -direction.x / length;
		direction.y = angle / length;
	}

	 else if (((Player_2_Points[4].y <= pos_2_y && pos_2_y <= Player_2_Points[0].y) || (Player_2_Points[4].y <= pos_4_y && pos_4_y <= Player_2_Points[0].y)) && Player_2_Points[4].x <= pos_2_x)
	{
		auto middle_position_ball_right_y = (pos_2_y + pos_4_y) / 2;
		auto middle_position_player_right_y = (Player_2_Points[0].y + Player_2_Points[4].y) / 2;
		float angle = (middle_position_ball_right_y - middle_position_player_right_y) / (fabs(Player_2_Points[4].y - Player_2_Points[0].y) / 2);
		float length = sqrt(direction.x * direction.x + angle * angle);
		direction.x = -direction.x / length;
		direction.y = angle/length;
	}
}

void Ball::UP_DOWN(Keys key, float velocity)
{
}

void Ball::DestroyResources()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pVertexShaderByteCode->Release();
	pPixelShaderByteCode->Release();
	pInputLayout->Release();
}

Ball::~Ball()
{
	pGame = nullptr;
	DestroyResources();
}

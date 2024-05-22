#include "GameStick.h"
#include <iostream>

GameStick::GameStick(Game* game) : pGame(game)
{
	CreateShadersAndInputLayout();

}

void GameStick::Initialize()
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

void GameStick::CreateShadersAndInputLayout()
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

void GameStick::Draw()
{
	pGame->GetDeviceContext()->DrawIndexed(pIndices.size(), 0, 0);
}

void GameStick::Reload()
{

}

void GameStick::Update(float deltaTime, float y)
{

}

DirectX::XMFLOAT4* GameStick::GetPoints()
{
	return pPoints.data();
}


void GameStick::CheckPlayersCollisions(GameComponent* player_1, GameComponent* player_2)
{

}

void GameStick::UP_DOWN(Keys key, float velocity)
{
	if (key == Keys::S && pPoints[4].y > -1.0f)
	{
		pPoints[0].y += velocity * -speed;
		pPoints[2].y += velocity * -speed;
		pPoints[4].y += velocity * -speed;
		pPoints[6].y += velocity * -speed;
	}
	else if (key == Keys::W && pPoints[0].y < 1.0f)
	{
		pPoints[0].y += velocity * speed;
		pPoints[2].y += velocity * speed;
		pPoints[4].y += velocity * speed;
		pPoints[6].y += velocity * speed;
	}
}

void GameStick::DestroyResources()
{
	pVertexShader->Release();
	pPixelShader->Release();
	pVertexShaderByteCode->Release();
	pPixelShaderByteCode->Release();
	pInputLayout->Release();
}

GameStick::~GameStick()
{
	pGame = nullptr;
	DestroyResources();
}

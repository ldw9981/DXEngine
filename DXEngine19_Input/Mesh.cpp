#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(LPCSTR fbxName, LPCWSTR vsName, LPCWSTR psName)
{
	this->fbxName = fbxName;
	vertexShader = VertexShader(vsName);
	pixelShader = PixelShader(psName);
}

Mesh::~Mesh()
{
}

bool Mesh::CompileShaders(ID3D11Device * device)
{
	// 컴파일.
	if (!vertexShader.CompileShader()) return false;
	if (!pixelShader.CompileShader()) return false;

	// 생성.
	if (!vertexShader.CreateShader(device)) return false;
	if (!pixelShader.CreateShader(device)) return false;

	return true;
}

void Mesh::BindShaders(ID3D11DeviceContext * deviceContext)
{
	vertexShader.BindShader(deviceContext);
	pixelShader.BindShader(deviceContext);
}

bool Mesh::CreateVertexBuffer(ID3D11Device * device)
{
	// 정점 개수 설정.
	int nVertices = vertices.size();

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	// sizeof(vertices) / sizeof(Vertex).
	vbDesc.ByteWidth = sizeof(Vertex) * nVertices;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.MiscFlags = 0;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;

	// 배열 데이터 할당.
	D3D11_SUBRESOURCE_DATA vbData;
	ZeroMemory(&vbData, sizeof(vbData));
	vbData.pSysMem = &vertices[0];

	// 정점 버퍼 생성.
	HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"정점 버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	return true;
}

void Mesh::BindVertexBuffer(ID3D11DeviceContext * deviceContext)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 정점 버퍼 바인딩.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
}

bool Mesh::CreateIndexBuffer(ID3D11Device * device)
{
	int nIndices = indices.size();

	D3D11_BUFFER_DESC ibDesc;
	ZeroMemory(&ibDesc, sizeof(D3D11_BUFFER_DESC));
	ibDesc.ByteWidth = sizeof(DWORD) * nIndices;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA ibData;
	ZeroMemory(&ibData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibData.pSysMem = &indices[0];

	// 인덱스 버퍼 생성.
	HRESULT hr = device->CreateBuffer(&ibDesc, &ibData, &indexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"인덱스 버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	return true;
}

void Mesh::BindIndexBuffer(ID3D11DeviceContext * deviceContext)
{
	// 인덱스 버퍼 바인딩(binding).
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

bool Mesh::CreateInputLayout(ID3D11Device * device)
{
	// 입력 레이아웃 생성.
	HRESULT hr = device->CreateInputLayout(
		inputLayoutDesc, 
		ARRAYSIZE(inputLayoutDesc),
		vertexShader.GetShaderBuffer()->GetBufferPointer(), 
		vertexShader.GetShaderBuffer()->GetBufferSize(), 
		&inputLayout);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"입력 레이아웃 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	return true;
}

void Mesh::BindInputLayout(ID3D11DeviceContext * deviceContext)
{
	// 입력 레이아웃 바인딩.
	deviceContext->IASetInputLayout(inputLayout);
}

void Mesh::AddTexture(Texture newTexture)
{
	pixelShader.AddTexture(newTexture);
}

void Mesh::DrawMesh(ID3D11DeviceContext * deviceContext)
{
	deviceContext->DrawIndexed(indices.size(), 0, 0);
}

XMFLOAT3 Mesh::GetPosition() const
{
	return position;
}

void Mesh::SetPosition(XMFLOAT3 position)
{
	this->position.x = position.x;
	this->position.y = position.y;
	this->position.z = position.z;
}

XMFLOAT3 Mesh::GetRotation() const
{
	return rotation;
}

void Mesh::SetRotation(XMFLOAT3 rotation)
{
	this->rotation.x = rotation.x;
	this->rotation.y = rotation.y;
	this->rotation.z = rotation.z;
}

XMFLOAT3 Mesh::GetScale() const
{
	return scale;
}

void Mesh::SetScale(XMFLOAT3 scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
}

XMMATRIX Mesh::GetTranslationMatrix()
{
	return XMMatrixTranslation(position.x, position.y, position.z);
}

//float rad2Deg = 180.0f / XM_PI;
//float deg2Rad = XM_PI / 180.0f;

XMMATRIX Mesh::GetRotationMatrix()
{	
	XMMATRIX rotX 
		= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	XMMATRIX rotY 
		= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	XMMATRIX rotZ 
		= XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	return rotZ * rotX * rotY;
}

XMMATRIX Mesh::GetScaleMarix()
{
	return XMMatrixScaling(scale.x, scale.y, scale.z);
}

XMMATRIX Mesh::GetWorldMatrix()
{
	return GetScaleMarix() * 
		GetRotationMatrix() * 
		GetTranslationMatrix();
}

bool Mesh::LoadTextures(ID3D11Device * device)
{
	if (!pixelShader.LoadTextures(device))
		return false;

	return true;
}

void Mesh::BindTextures(ID3D11DeviceContext * deviceContext)
{
	pixelShader.BindTextures(deviceContext);
}

bool Mesh::CreateSamplerState(ID3D11Device * device)
{
	if (!pixelShader.CreateSamplerState(device))
		return false;

	return true;
}

void Mesh::BindSamplerState(ID3D11DeviceContext * deviceContext)
{
	pixelShader.BindSamplerState(deviceContext);
}

void Mesh::Release()
{
	Memory::SafeRelease(vertexBuffer);
	Memory::SafeRelease(indexBuffer);
	Memory::SafeRelease(inputLayout);

	vertexShader.Release();
	pixelShader.Release();
}
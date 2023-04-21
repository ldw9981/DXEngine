#pragma once

#include "DXUtil.h"
#include <vector>
#include "VertexShader.h"
#include "PixelShader.h"

class Mesh
{
public:
	Mesh();
	Mesh(LPCSTR fbxName, LPCWSTR vsName, LPCWSTR psName);
	~Mesh();

	bool CompileShaders(ID3D11Device* device);
	void BindShaders(ID3D11DeviceContext* deviceContext);

	// 정점 버퍼 생성&바인딩.
	bool CreateVertexBuffer(ID3D11Device* device);
	void BindVertexBuffer(ID3D11DeviceContext* deviceContext);

	// 인덱스 버퍼 생성 & 바인딩.
	bool CreateIndexBuffer(ID3D11Device* device);
	void BindIndexBuffer(ID3D11DeviceContext* deviceContext);

	// 입력 레이아웃.
	bool CreateInputLayout(ID3D11Device* device);
	void BindInputLayout(ID3D11DeviceContext* deviceContext);

	void AddTexture(Texture newTexture);

	// 그리기.
	void DrawMesh(ID3D11DeviceContext* deviceContext);

	// Getter / Setter.
	XMFLOAT3 GetPosition() const;
	void SetPosition(XMFLOAT3 position);

	XMFLOAT3 GetRotation() const;
	void SetRotation(XMFLOAT3 rotation);

	XMFLOAT3 GetScale() const;
	void SetScale(XMFLOAT3 scale);

	XMMATRIX GetTranslationMatrix();
	XMMATRIX GetRotationMatrix();
	XMMATRIX GetScaleMarix();

	LPCSTR GetFBXName() const { return fbxName; }
	
	std::vector<Vertex>* GetVertexArray() { return &vertices; }
	int GetVertexCount() const { return vertices.size(); }

	std::vector<DWORD>* GetIndexArray() { return &indices; }
	int GetIndexCount() const { return indices.size(); }

	D3D11_INPUT_ELEMENT_DESC* GetInputLayoutDesc() { return inputLayoutDesc; }

	// 추가되는 부분.
	CBPerObject GetWVPMatrices() const { return vertexShader.GetWVPMaticex(); }
	void SetWVPMatrices(
		XMMATRIX world,
		XMMATRIX view,
		XMMATRIX projection
	) {
		vertexShader.SetWVPMatrices(world, view, projection);
	}

	XMMATRIX GetWorldMatrix();

	CBLight GetLightInfo() const 
	{ 
		return vertexShader.GetLightInfo(); 
	}
	void SetLightInfo(XMVECTOR lightPos, XMVECTOR camPos) 
	{ 
		vertexShader.SetLightInfo(lightPos, camPos); 
	}

	bool LoadTextures(ID3D11Device* device);
	void BindTextures(ID3D11DeviceContext* deviceContext);
	bool CreateSamplerState(ID3D11Device* device);
	void BindSamplerState(ID3D11DeviceContext* deviceContext);

	void Release();




private:

	LPCSTR fbxName;						// 적재(load)할 FBX 이름.
	std::vector<Vertex> vertices;		// 정점 배열.
	std::vector<DWORD> indices;		// 인덱스 배열.
	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[5] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 버퍼.
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;

	// 셰이더 (정점/픽셀).
	VertexShader vertexShader;
	PixelShader pixelShader;

	// 트랜스폼 정보.
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
};
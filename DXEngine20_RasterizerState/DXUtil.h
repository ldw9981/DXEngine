#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <xnamath.h>

#include <fbxsdk.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "libfbxsdk.lib")

// 메모리 해제 헬퍼 함수.
namespace Memory
{
	template <class T> void SafeRelease(T& t)
	{
		if (t)
		{
			t->Release();
			t = NULL;
		}
	}

	template <class T> void SafeDelete(T& t)
	{
		if (t)
		{
			delete t;
			t = NULL;
		}
	}

	template <class T> void SafeDeleteArray(T& t)
	{
		if (t)
		{
			delete[] t;
			t = NULL;
		}
	}
}

// 정점 선언.
struct Vertex
{
	XMFLOAT3 position;		// 정점 위치 정보.
	XMFLOAT2 texCoord;		// 텍스처 좌표(UV).
	XMFLOAT3 normal;		// 노멀 정보.
	XMFLOAT3 tangent;		// 탄젠트.
	XMFLOAT3 binormal;		// 바이노멀.

	Vertex() { }
	Vertex(float x, float y, float z) : position(x, y, z) { }
	Vertex(XMFLOAT3 position) : position(position) { }

	Vertex(XMFLOAT3 position,
		XMFLOAT2 texCoord,
		XMFLOAT3 normal) : position(position), texCoord(texCoord),
		normal(normal) { }
};

// 상수 버퍼용 구조체.
struct CBPerObject
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

// 상수 버퍼 구조체 - 라이트 정보.
struct CBLight
{
	XMVECTOR lightPosition;
	XMVECTOR cameraPosition;
};

struct Texture
{
public:
	Texture() { ZeroMemory(this, sizeof(Texture)); }
	~Texture() { Memory::SafeRelease(textureResource); }

	LPCWSTR fileName;
	ID3D11ShaderResourceView* textureResource = NULL;
};

bool IsError(HRESULT hr, LPCWSTR errorMsg);

extern float Rad2Deg;
extern float Deg2Rad;
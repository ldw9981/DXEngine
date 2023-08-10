#pragma once

#include <D3D11.h>
#include <directxtk/simplemath.h>
#include <fbxsdk.h>

#pragma comment(lib, "d3d11.lib")
 
#pragma comment(lib, "libfbxsdk.lib")


using namespace DirectX::SimpleMath;
using namespace DirectX;


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
	Vector3 position;		// 정점 위치 정보.
	Vector2 texCoord;		// 텍스처 좌표(UV).
	Vector3 normal;		// 노멀 정보.
	Vector3 tangent;		// 탄젠트.
	Vector3 binormal;		// 바이노멀.

	Vertex() { }
	Vertex(float x, float y, float z) : position(x, y, z) { }
	Vertex(Vector3 position) : position(position) { }

	Vertex(Vector3 position,
		Vector2 texCoord,
		Vector3 normal) : position(position), texCoord(texCoord),
		normal(normal) { }
};

// 상수 버퍼용 구조체.
struct CBPerObject
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

// 상수 버퍼 구조체 - 라이트 정보.
struct CBLight
{
	Vector4 lightPosition;
	Vector4 cameraPosition;
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
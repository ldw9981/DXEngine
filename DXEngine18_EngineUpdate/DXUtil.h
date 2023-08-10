#pragma once

#include <D3D11.h>
#include <fbxsdk.h>
#include <directxtk/simplemath.h>

#pragma comment(lib, "d3d11.lib") 
#pragma comment(lib, "libfbxsdk.lib")

using namespace DirectX::SimpleMath;
using namespace DirectX;

// �޸� ���� ���� �Լ�.
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
// ���� ����.
struct Vertex
{
	Vector3 position;		// ���� ��ġ ����.
	Vector2 texCoord;		// �ؽ�ó ��ǥ(UV).
	Vector3 normal;		// ��� ����.
	Vector3 tangent;		// ź��Ʈ.
	Vector3 binormal;		// ���̳��.

	Vertex() { }
	Vertex(float x, float y, float z) : position(x, y, z) { }
	Vertex(Vector3 pos) : position(pos) { }

	Vertex(Vector3 pos,
		Vector2 tex,
		Vector3 normal) : position(pos), texCoord(tex),
		normal(normal) { }
};

// ��� ���ۿ� ����ü.
struct CBPerObject
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

// ��� ���� ����ü - ����Ʈ ����.
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
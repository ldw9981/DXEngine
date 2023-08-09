#pragma once


#include <Windows.h>
#include "DXUtil.h"

#include <d3d11.h>
#include <directxtk/simplemath.h>
#include <directxtk/WICTextureLoader.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTK.lib")

using namespace DirectX::SimpleMath;
using namespace DirectX;

class DXApp
{
public:
	// ���� ����.
	struct Vertex
	{
		Vector3 position;		// ���� ��ġ ����.
		//Vector4 color;			// ���� ���� ����.
		Vector2 texCoord;		// �ؽ�ó ��ǥ(UV).
		Vector3 normal;		// ��� ����.

		Vertex() { }
		Vertex(float x, float y, float z) : position(x, y, z) { }
		Vertex(Vector3 position) : position(position) { }

		Vertex(Vector3 position, 
			Vector2 texCoord,
			Vector3 normal) : position(position), texCoord(texCoord),
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
		XMVECTOR lightPosition;
	};

	DXApp(HINSTANCE hinstance);
	virtual ~DXApp();

	int Run();

	virtual bool Init();
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual LRESULT MSGProc(HWND, UINT, WPARAM, LPARAM);

protected:

	// Win32 ���� ����.
	HWND hwnd;
	HINSTANCE hinstance;
	UINT clientWidth;
	UINT clientHeight;
	LPCWSTR applicationName;
	DWORD wndStyle;

	// DirectX ����.
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;

	ID3D11Buffer* vertexBuffer;		// ���� ����.
	ID3D11VertexShader* vertexShader;		// ���� ���̴�.
	ID3D11PixelShader* pixelShader;			// �ȼ� ���̴�.
	ID3DBlob* vertexShaderBuffer;		// ���� ���̴� ����.
	ID3DBlob* pixelShaderBuffer;		// �ȼ� ���̴� ����.
	ID3D11InputLayout* vertexInputLayout;	// �Է� ���̾ƿ�.

	int nVertices;							// ���� ����.
	Vertex* vertices;						// ���� �迭.

	ID3D11Buffer* indexBuffer;			// �ε���(����) ����.
	int nIndices;								// �ε��� ����.
	DWORD* indices;						// �ε��� �迭.

	// ������ȯ�� �ʿ��� ���� ����.
	Matrix worldMatrix;				// ���� ��ȯ ���.
	Matrix viewMatrix;				// �� ��ȯ ���.
	Matrix projectionMatrix;		// ���� ��ȯ ���.

	XMVECTOR cameraPos;				// ī�޶� ��ġ ����.
	XMVECTOR cameraTarget;			// ī�޶� �ٶ� ���� ����.
	XMVECTOR cameraUp;				// ī�޶� �� ���� ����.

	ID3D11Buffer* cBuffer;				// �������.

	// �ؽ�ó / ���÷� ������Ʈ.
	ID3D11ShaderResourceView* pTexture;
	ID3D11SamplerState* pSamplerState;

	ID3D11Buffer* lightBuffer;		// ������� - ����Ʈ��.

protected:

	// �ʱ�ȭ �Լ�(�޼ҵ�).
	bool InitWindow();

	// DirectX �ʱ�ȭ �Լ�.
	bool InitDirect3D();

	// DirectX ��� �ʱ�ȭ.
	bool InitScene();

	// �� ���� �ʱ�ȭ (ť��).
	bool LoadModel(const char* fileName);

	// ���� ��ȯ ��� �ʱ�ȭ.
	bool InitTransformation();

	// �ؽ�ó �ʱ�ȭ.
	bool InitTexture();

	// ����Ʈ ���� �ʱ�ȭ.
	bool InitLightCB();
};
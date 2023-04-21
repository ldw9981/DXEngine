#pragma once

#include <Windows.h>
#include "DXUtil.h"

class DXApp
{
public:
	// 정점 선언.
	struct Vertex
	{
		XMFLOAT3 position;		// 정점 위치 정보.
		XMFLOAT4 color;			// 정점 색상 정보.

		Vertex(float x, float y, float z) : position(x, y, z) { }
		Vertex(XMFLOAT3 position) : position(position) { }
		
		Vertex(XMFLOAT3 position, XMFLOAT4 color) 
			: position(position), color(color) { }
	};

	DXApp(HINSTANCE hinstance);
	virtual ~DXApp();

	int Run();

	virtual bool Init();
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual LRESULT MSGProc(HWND, UINT, WPARAM, LPARAM);

protected:

	// Win32 설정 변수.
	HWND hwnd;
	HINSTANCE hinstance;
	UINT clientWidth;
	UINT clientHeight;
	LPCWSTR applicationName;
	DWORD wndStyle;

	// DirectX 변수.
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;

	ID3D11Buffer* vertexBuffer;		// 정점 버퍼.
	ID3D11VertexShader* vertexShader;		// 정점 셰이더.
	ID3D11PixelShader* pixelShader;			// 픽셀 셰이더.
	ID3DBlob* vertexShaderBuffer;		// 정점 셰이더 버퍼.
	ID3DBlob* pixelShaderBuffer;		// 픽셀 셰이더 버퍼.
	ID3D11InputLayout* vertexInputLayout;	// 입력 레이아웃.

	int nVertices;							// 정점 개수.

protected:

	// 초기화 함수(메소드).
	bool InitWindow();

	// DirectX 초기화 함수.
	bool InitDirect3D();

	// DirectX 장면 초기화.
	bool InitScene();
};
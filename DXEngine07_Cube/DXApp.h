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
		//XMFLOAT4 color;			// 정점 색상 정보.
		XMFLOAT2 texCoord;		// 텍스처 좌표(UV).
		XMFLOAT3 normal;		// 노멀 정보.

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
	Vertex* vertices;						// 정점 배열.

	ID3D11Buffer* indexBuffer;			// 인덱스(색인) 버퍼.
	int nIndices;								// 인덱스 개수.
	DWORD* indices;						// 인덱스 배열.

	// 공간변환에 필요한 변수 선언.
	XMMATRIX worldMatrix;				// 월드 변환 행렬.
	XMMATRIX viewMatrix;				// 뷰 변환 행렬.
	XMMATRIX projectionMatrix;		// 투영 변환 행렬.

	XMVECTOR cameraPos;				// 카메라 위치 저장.
	XMVECTOR cameraTarget;			// 카메라가 바라볼 방향 저장.
	XMVECTOR cameraUp;				// 카메라 위 방향 저장.

	ID3D11Buffer* cBuffer;				// 상수버퍼.

	// 텍스처 / 샘플러 스테이트.
	ID3D11ShaderResourceView* pTexture;
	ID3D11SamplerState* pSamplerState;

protected:

	// 초기화 함수(메소드).
	bool InitWindow();

	// DirectX 초기화 함수.
	bool InitDirect3D();

	// DirectX 장면 초기화.
	bool InitScene();

	// 모델 정보 초기화 (큐브).
	bool LoadModel(const char* fileName);

	// 공간 변환 행렬 초기화.
	bool InitTransformation();

	// 텍스처 초기화.
	bool InitTexture();
};
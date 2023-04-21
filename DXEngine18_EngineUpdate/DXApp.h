#pragma once

#include <Windows.h>
#include "DXUtil.h"
#include "GameTimer.h"
#include <vector>
#include "Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "FBXLoader.h"

class DXApp
{
public:
	DXApp(HINSTANCE hinstance);
	virtual ~DXApp();

	int Run();

	virtual bool Init();
	virtual void Update(double deltaTime) = 0;
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

	// 뎁스/스텐실 버퍼.
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

	// 공간변환에 필요한 변수 선언.
	XMMATRIX worldMatrix;				// 월드 변환 행렬.
	XMMATRIX viewMatrix;				// 뷰 변환 행렬.
	XMMATRIX projectionMatrix;		// 투영 변환 행렬.

	XMVECTOR cameraPos;				// 카메라 위치 저장.
	XMVECTOR cameraTarget;			// 카메라가 바라볼 방향 저장.
	XMVECTOR cameraUp;				// 카메라 위 방향 저장.

	ID3D11Buffer* cBuffer;				// 상수버퍼 - WVP.
	ID3D11Buffer* lightBuffer;		// 상수버퍼 - 라이트용.
	GameTimer gameTimer;			// 게임 타이머.

	// 메쉬.
	std::vector<Mesh> meshes;

protected:

	// 초기화 함수(메소드).
	bool InitWindow();

	// DirectX 초기화 함수.
	bool InitDirect3D();

	// 뎁스 버퍼 초기화.
	bool InitDepthStencilBuffer();

	// DirectX 장면 초기화.
	bool InitScene();

	// 메쉬 초기화.
	void InitMeshInfo();

	// 컴파일/바인딩 셰이더.
	bool CompileShader(Mesh* mesh);
	void BindShader(Mesh* mesh);

	// 정점 버퍼 설정.
	bool InitVertexBuffer(Mesh* mesh);
	void BindVertexBuffer(Mesh* mesh);

	// 인덱스 버퍼 설정.
	bool InitIndexBuffer(Mesh* mesh);
	void BindIndexBuffer(Mesh* mesh);

	// 입력 레이아웃 설정.
	bool InitInputLayout(Mesh* mesh);
	void BindInputLayout(Mesh* mesh);

	// 공간 변환 행렬 초기화.
	bool InitTransformation();
	bool InitTransformation(Mesh* mesh);

	void InitWorldMatrix(Mesh* mesh);
	void InitViewMatrix();
	void InitProjectionMatrix();

	bool InitWVPBuffer(Mesh* mesh);
	void UpdateWVPBuffer(Mesh* mesh);

	// 라이트 정보 초기화.
	bool InitLightCB();

	// 라이트 정보 갱신.
	void UpdateLightCB(Mesh* mesh);

	// 라이트 정보 바인딩.
	void BindLightCB();

	// 뷰포트 설정.
	void SetViewport();
};
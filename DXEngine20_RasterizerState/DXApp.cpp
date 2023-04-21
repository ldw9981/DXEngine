#include "DXApp.h"
#include <fstream>

using namespace std;;

// 글로벌 변수.
DXApp* pApp = NULL;

// 메시지 처리 콜백 함수 (래퍼, wrapper).
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	if (pApp != NULL) return pApp->MSGProc(hwnd, msg, wParam, lParam);
	else return DefWindowProc(hwnd, msg, wParam, lParam);
}

DXApp::DXApp(HINSTANCE hinstance)
{
	// 초기화.
	hwnd = NULL;
	this->hinstance = hinstance;
	clientWidth = 1600;
	clientHeight = 960;
	applicationName = L"Engine20 - RasterizerState";
	wndStyle = WS_OVERLAPPEDWINDOW;

	pApp = this;

	pDevice = NULL;
	pDeviceContext = NULL;
	pSwapChain = NULL;
	pRenderTargetView = NULL;

	
}

DXApp::~DXApp()
{
	// 메모리 해제.
	Memory::SafeRelease(pDevice);
	Memory::SafeRelease(pDeviceContext);
	Memory::SafeRelease(pRenderTargetView);
	Memory::SafeRelease(pSwapChain);

	Memory::SafeRelease(cBuffer);

	Memory::SafeRelease(lightBuffer);

	Memory::SafeRelease(depthStencilView);
	Memory::SafeRelease(depthStencilBuffer);

	for (int ix = 0; ix < meshes.size(); ++ix)
	{
		meshes[ix].Release();
	}
}

// 메시지 처리 루프.
int DXApp::Run()
{
	// 메시지 처리 루프.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 루프 실행.
	while (msg.message != WM_QUIT)
	{
		// 메시지 매핑.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 타이머 업데이트.
			gameTimer.UpdateTimer(this);

			// 델타 타임(프레임 시간) 계산.
			double deltaTime = gameTimer.DeltaTime();

			ProcessInput(deltaTime);

			// 엔진 업데이트.
			Update(deltaTime);

			// 화면 그리기.
			Render();
		}
	}

	return 0;
}

// 초기화 메소드.
bool DXApp::Init()
{
	// Win32 초기화.
	if (InitWindow() == false)
		return false;

	// Direct3D 초기화.
	if (InitDirect3D() == false)
		return false;
	
	// 타이머 초기화.
	gameTimer.StartTime();

	// 장면 초기화.
	if (InitScene() == false) 
		return false;

	// 공간 변환 행렬 초기화.
	//if (InitTransformation() == false) return false;

	// 라이트 버퍼 초기화.
	if (InitLightCB() == false)
		return false;

	// 입력 초기화.
	if (!InitInput())
		return false;

	return true;
}

LRESULT DXApp::MSGProc(HWND hwnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	// 메시지 처리.
	switch (msg)
	{
		//case WM_KEYUP
	case WM_KEYDOWN:
	{
		// ESC가 눌리면.
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
		}
		return 0;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

// 창 초기화.
bool DXApp::InitWindow()
{
	// 윈도우 클래스 생성.
	WNDCLASSEX wc;
	// 초기화.
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"WindowClass";
	wc.lpfnWndProc = WinProc;

	// 클래스 등록.
	if (!RegisterClassEx(&wc))
		return false;

	// 윈도우 생성.
	//HWND hwnd;
	hwnd = CreateWindow(L"WindowClass", applicationName,
		wndStyle, 0, 0, clientWidth, clientHeight, NULL, NULL,
		hinstance, NULL);

	// 예외 처리.
	if (hwnd == NULL)
		return false;

	// 창 띄우기.
	ShowWindow(hwnd, SW_SHOW);

	return true;
}

bool DXApp::InitInput()
{
	input = InputClass(hinstance, hwnd);
	
	if (!input.InitInput()) 
		return false;

	return true;
}

bool DXApp::InitDirect3D()
{
	// 결과값.
	HRESULT hr;

	// 스왑체인 속성 설정 구조체 생성.
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hwnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = clientWidth;
	swapDesc.BufferDesc.Height = clientHeight;
	// 화면 주사율 설정.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// 샘플링 관련 설정.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	// 장치 및 스왑체인 생성.
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &pSwapChain, &pDevice,
		NULL, &pDeviceContext);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"장치 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 백버퍼(텍스처).
	ID3D11Texture2D* pBackBufferTexture;
	hr = pSwapChain->GetBuffer(NULL,
		__uuidof(ID3D11Texture2D),
		(void**)&pBackBufferTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"백버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 렌더 타겟 생성.
	hr = pDevice->CreateRenderTargetView(
		pBackBufferTexture, NULL, &pRenderTargetView);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"렌더 타겟 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 뎁스/스텐실 뷰 생성.
	if (InitDepthStencilBuffer() == false)
		return false;

	// 렌더 타겟 설정.
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, depthStencilView);

	// 백버퍼 텍스처 해제.
	if (pBackBufferTexture)
	{
		pBackBufferTexture->Release();
		pBackBufferTexture = NULL;
	}

	return true;
}

bool DXApp::InitDepthStencilBuffer()
{
	// 버퍼 서술자 설정.
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Width = clientWidth;
	depthDesc.Height = clientHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	// 뎁스/스텐실 버퍼 생성.
	HRESULT hr;
	hr = pDevice->CreateTexture2D(&depthDesc, NULL, &depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"뎁스/스텐실 버퍼 생성 실패.", L"오류", MB_OK);
		return false;
	}

	// 뎁스/스텐실 뷰 생성.
	hr = pDevice->CreateDepthStencilView(
		depthStencilBuffer, NULL, &depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"뎁스/스텐실 뷰 생성 실패.", L"오류", MB_OK);
		return false;
	}

	return true;
}

bool DXApp::InitScene()
{
	// 메쉬 정보 설정.
	InitMeshInfo();

	for (int ix = 0; ix < meshes.size(); ++ix)
	{
		Mesh* mesh = &meshes[ix];

		if (!CompileShader(mesh)) return false;

		if (!InitVertexBuffer(mesh)) return false;

		if (!InitIndexBuffer(mesh)) return false;

		if (!InitInputLayout(mesh)) return false;

		if (!mesh->CreateRasterizerState(pDevice, D3D11_FILL_WIREFRAME, D3D11_CULL_BACK))
			return false;

		mesh->BindRasterizerState(pDeviceContext);

		// 정점을 이어서 그릴 방식 설정.
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (!mesh->LoadTextures(pDevice)) return false;

		if (!mesh->CreateSamplerState(pDevice)) return false;

		InitTransformation(mesh,&camera);

		InitLightCB();

		BindLightCB();
	}

	SetViewport();
	

	return true;
}

void DXApp::InitMeshInfo()
{
	// 모델(FBX) 이름.
	LPCSTR fbxNameTPP 
		= "Resources//Models//HeroTPP.fbx";
	LPCSTR fbxNameBox
		= "Resources//Models//SK_CharM_Cardboard.fbx";
	LPCSTR fbxNameSphere
		= "Resources//Models//sphere.fbx";

	// 텍스처 이름.
	Texture tppDiffuseMap;
	tppDiffuseMap.fileName 
		= L"Resources//Textures//T_Chr_FPS_D.png";
	
	Texture tppNormalMap;
	tppNormalMap.fileName
		= L"Resources//Textures//T_Chr_FPS_N.png";

	Texture boxDiffuseMap;
	boxDiffuseMap.fileName
		= L"Resources//Textures//Char_M_Cardboard_D.png";
	
	Texture boxNormalMap;
	boxNormalMap.fileName
		= L"Resources//Textures//Char_M_Cardboard_N.png";

	Texture cubeMap;
	cubeMap.fileName 
		= L"Resources//Textures//LancellottiChapel.dds";


	// 모델(Mesh).
	Mesh tppDiffuse(
		fbxNameTPP, 
		L"Shaders//DiffuseVS.fx", 
		L"Shaders//DiffusePS.fx"
	);
	// 텍스처 추가.
	tppDiffuse.AddTexture(tppDiffuseMap);
	tppDiffuse.SetPosition(XMFLOAT3(0.0f, -90.0f, 0.0f));
	tppDiffuse.SetRotation(XMFLOAT3(-90.0f, 180.0f, 0.0f));

	// 메시 배열에 추가.
	meshes.push_back(tppDiffuse);

	Mesh tppNormal(
		fbxNameTPP, 
		L"Shaders//NormalVS.fx", 
		L"Shaders//NormalPS.fx");
	tppNormal.AddTexture(tppDiffuseMap);
	tppNormal.AddTexture(tppNormalMap);
	tppNormal.SetPosition(XMFLOAT3(-150.0f, -90.0f, 0.0f));
	tppNormal.SetRotation(XMFLOAT3(-90.0f, 180.0f, 0.0f));
	meshes.push_back(tppNormal);

	Mesh boxNormal(
		fbxNameBox, 
		L"Shaders//NormalVS.fx", 
		L"Shaders//NormalPS.fx");
	boxNormal.AddTexture(boxDiffuseMap);
	boxNormal.AddTexture(boxNormalMap);
	boxNormal.SetPosition(XMFLOAT3(150.0f, -90.0f, 0.0f));
	boxNormal.SetRotation(XMFLOAT3(-90.0f, 180.0f, 0.0f));
	meshes.push_back(boxNormal);
}

bool DXApp::CompileShader(Mesh * mesh)
{
	if (!mesh->CompileShaders(pDevice)) return false;
	return true;
}

void DXApp::BindShader(Mesh * mesh)
{
	mesh->BindShaders(pDeviceContext);
}

bool DXApp::InitVertexBuffer(Mesh * mesh)
{
	// 정점 정보를 FBX로부터 로드.
	HRESULT hr;
	hr = FBXLoader::LoadFBX(
		mesh->GetFBXName(),
		mesh->GetVertexArray(),
		mesh->GetIndexArray()
	);
	if (FAILED(hr)) return false;

	// 정점 버퍼 생성.
	if (!mesh->CreateVertexBuffer(pDevice)) 
		return false;

	return true;
}

void DXApp::BindVertexBuffer(Mesh * mesh)
{
	mesh->BindVertexBuffer(pDeviceContext);
}

bool DXApp::InitIndexBuffer(Mesh * mesh)
{
	if (!mesh->CreateIndexBuffer(pDevice)) 
		return false;

	return true;
}

void DXApp::BindIndexBuffer(Mesh * mesh)
{
	mesh->BindIndexBuffer(pDeviceContext);
}

bool DXApp::InitInputLayout(Mesh * mesh)
{
	if (!mesh->CreateInputLayout(pDevice))
		return false;

	return true;
}

void DXApp::BindInputLayout(Mesh * mesh)
{
	mesh->BindInputLayout(pDeviceContext);
}


bool DXApp::InitTransformation(Mesh * mesh, CameraClass* camera)
{
	// 카메라 위치 설정.
	camera->SetPosition(XMVectorSet(0.0f, 0.0f, -200.0f, 0.0f));

	// 행렬 설정.
	mesh->SetWVPMatrices(
		mesh->GetWorldMatrix(),
		camera->GetViewMatrix(),
		camera->GetProjectionMatrix()
	);

	InitWVPBuffer(mesh);

	pDeviceContext->VSSetConstantBuffers(0, 1, &cBuffer);

	return true;
}

bool DXApp::InitWVPBuffer(Mesh * mesh)
{
	// 버퍼 설정.
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(CBPerObject);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.pSysMem = &mesh->GetWVPMatrices();

	// 버퍼 생성.
	HRESULT hr = pDevice->CreateBuffer(&cbDesc, &cbData, &cBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"상수 버퍼 생성 실패.", L"오류", MB_OK);
		return false;
	}

	// WVP용 상수버퍼 바인딩.
	//pDeviceContext->VSSetConstantBuffers(0, 1, &cBuffer);

	return true;
}

void DXApp::UpdateWVPBuffer(
	Mesh * mesh, 
	CameraClass* camera)
{
	// WVP 행렬 값 저장.
	mesh->SetWVPMatrices(
		mesh->GetWorldMatrix(),
		camera->GetViewMatrix(), 
		camera->GetProjectionMatrix()
	);

	// 갱신된 행렬 값으로 바인딩된 버퍼 업데이트.
	pDeviceContext->UpdateSubresource(cBuffer, 0, 0, &mesh->GetWVPMatrices(), 0, 0);
}

bool DXApp::InitLightCB()
{
	// 라이트 상수 버퍼용 구조체 설정.
	CBLight cbLight;
	ZeroMemory(&cbLight, sizeof(cbLight));
	cbLight.lightPosition = XMVectorSet(500.0f, 500.0f, -500.0f, 1.0f);
	cbLight.cameraPosition = camera.GetPosition();


	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(CBLight);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.pSysMem = &cbLight;

	// 상수 버퍼 생성.
	HRESULT hr = pDevice->CreateBuffer(&cbDesc, &cbData, &lightBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"라이트 버퍼 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 바인딩.
	pDeviceContext->VSSetConstantBuffers(1, 1, &lightBuffer);

	return true;
}

void DXApp::UpdateLightCB(Mesh * mesh)
{
	pDeviceContext->UpdateSubresource(lightBuffer, 0, 0, &mesh->GetLightInfo(), 0, 0);
}

void DXApp::BindLightCB()
{
	pDeviceContext->VSSetConstantBuffers(1, 1, &lightBuffer);
}

void DXApp::SetViewport()
{
	// 뷰포트 설정.
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = clientWidth;
	viewport.Height = clientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// 뷰포트 설정.
	pDeviceContext->RSSetViewports(1, &viewport);

	camera.SetAspecRation(clientWidth, clientHeight);
}
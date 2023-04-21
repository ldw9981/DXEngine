#include <Windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <fbxsdk.h>
#include <dinput.h>
using namespace std;

#pragma comment(lib, "libfbxsdk.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


#define DIRECTINPUT_VERSION 0x0800

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

// 상수 버퍼 구조체 - 라이트 정보.
struct CBLight
{
	XMVECTOR lightPosition;
	XMVECTOR cameraPosition;
};

CBPerObject cbMatrices;
CBLight cbLight;


HWND hwnd;
HINSTANCE hInstance;
UINT clientWidth = 800;
UINT clientHeight = 600;
LPCWSTR applicationName = PROJECT_NAME;
DWORD wndStyle = WS_OVERLAPPEDWINDOW;

// Device , Context
ID3D11Device* pDevice;
ID3D11DeviceContext* pDeviceContext;
IDXGISwapChain* pSwapChain;
ID3D11RenderTargetView* pRenderTargetView;

// 뎁스/스텐실 버퍼.
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

// Geometry , Shader
ID3D11Buffer* vertexBuffer;
ID3D11VertexShader* vertexShader;
ID3D11PixelShader* pixelShader;
ID3DBlob* vertexShaderBuffer;
ID3DBlob* pixelShaderBuffer;
ID3D11InputLayout* vertexInputLayout;
int nVertices;
ID3D11Buffer* indexBuffer;
int nIndices;

std::vector<Vertex> vertices;		// 정점 배열.
std::vector<DWORD> indices;			// 인덱스 배열.







ID3D11Buffer* cBuffer;
ID3D11Buffer* lightBuffer;		// 상수버퍼 - 라이트용.

// Texture
ID3D11ShaderResourceView* pTexture;
ID3D11SamplerState* pSamplerState;
FbxManager* fbxManager;		// FBX 로더.

//인풋
IDirectInputDevice8* diKeyboard;
IDirectInputDevice8* diMouse;

DIMOUSESTATE currentMouseState;
DIMOUSESTATE prevMouseState;
LPDIRECTINPUT8 directInput;

// 타이머 관련 변수.
double countsPerSecond = 0.0;
__int64 countStart = 0;

int frameCount = 0;
int fps = 0;

__int64 prevFrameTime = 0;

//카메라정보
float fovY;				//시야각.
float aspectRatio;		// 화면 종횡비.
float nearZ =1.0f ;				// 근평면 거리.
float farZ=1000.f ;				// 원평면 거리.

float yaw = 0.0f;		// yaw 회전 값.
float pitch = 0.0f;		// pitch 회전 값.

float moveSpeed = 200.0f;		// 카메라 이동 속도.
float rotationSpeed = 30.0f;		// 카메라 회전 속도.

bool modifiedProjection = false;

// Transform
XMMATRIX worldMatrix;

//카메라
XMMATRIX viewMatrix;
XMMATRIX projectionMatrix;

XMVECTOR cameraPosition;
XMVECTOR cameraLook
= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR cameraRight
= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR cameraForward
= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

XMVECTOR defaultRight
= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR defaultUp
= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR defaultForward
= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);




// util
bool IsError(HRESULT hr, LPCWSTR errorMsg)
{
	if (hr < 0)
	{
		MessageBox(NULL, errorMsg, L"오류", MB_OK);
		return true;
	}

	return false;
}

float Rad2Deg = 180.0f / XM_PI;
float Deg2Rad = XM_PI / 180.0f;



void StartTime()
{
	LARGE_INTEGER frequencyCount;

	// 하드웨어 진동수(hz) 저장.
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = (double)frequencyCount.QuadPart;

	// 틱(Tick = Frame).
	QueryPerformanceCounter(&frequencyCount);
	countStart = frequencyCount.QuadPart;
}

double GetTime() 
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	double time
		= (double)((currentTime.QuadPart - countStart)
			/ countsPerSecond);

	return time;
}

double DeltaTime()
{
	// 틱 단위 현재 시간 구하기.
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	// 현재 프레임에서 이전 프레임까지 틱 단위로 얼마나 지났는지 계산.
	__int64 tickCount = currentTime.QuadPart - prevFrameTime;

	// 이전 프레임 시간 저장.
	prevFrameTime = currentTime.QuadPart;

	// 예외 처리.
	if (tickCount < 0.0f)
		tickCount = 0.0f;

	// 프레임 시간을 초(단위) 시간으로 변환.
	double deltaTime = (double)(tickCount / countsPerSecond);

	// 예외 처리.
	if (deltaTime < 0.0)
		deltaTime = 0.0;

	return deltaTime;
}
void Reset()
{
	fps = frameCount;
	frameCount = 0;
	StartTime();

	// FPS 콘솔에 출력.
	OutputDebugString(L"FPS: ");
	wchar_t buffer[256];
	_itow_s(fps, buffer, 10);
	OutputDebugString(buffer);
	OutputDebugString(L"\n");
}
void UpdateTimer()
{
	// 틱 단위로 프레임 수 증가시키기.
	++frameCount;

	// 1초가 지나면 타이머 초기화.
	if (GetTime() > 1.0f)
		Reset();
}



int GetFPS()
{
	return fps;
}


XMFLOAT2 ReadUV(FbxMesh * mesh, int controlPointIndex, int vertexCounter)
{
	// UV가 있는지 확인.
	if (mesh->GetElementUVCount() < 1)
	{
		MessageBox(NULL, L"UV가 없습니다.", L"오류", MB_OK);
		return NULL;
	}

	// 반환용 데이터 선언.
	XMFLOAT2 texCoord(0.0f, 0.0f);

	// UV 전체 배열 읽기.
	FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);
	const bool isUsingIndex
		= vertexUV->GetReferenceMode() != FbxGeometryElement::eDirect;
	const int indexCount = isUsingIndex ? vertexUV->GetIndexArray().GetCount() : 0;

	// 모드 확인.
	switch (vertexUV->GetMappingMode())
	{
		// 현재 정점이 제어점 유형인 경우.
	case FbxGeometryElement::eByControlPoint:
	{
		// 현재 UV 값을 읽어올 인덱스 얻어오기.
		int index = isUsingIndex ? vertexUV->GetIndexArray().GetAt(controlPointIndex) : controlPointIndex;

		/*if (isUsingIndex == true)
			index = vertexUV->GetIndexArray().GetAt(controlPointIndex);
		else
			index = controlPointIndex;*/

			// UV 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
		texCoord.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
		texCoord.y = 1.0f - (float)vertexUV->GetDirectArray().GetAt(index).mData[1];

		// UV 값 반환.
		return texCoord;
	}

	case FbxGeometryElement::eByPolygonVertex:
	{
		// 현재 UV 값을 읽어올 인덱스 얻어오기.
		int index = isUsingIndex ? vertexUV->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

		// UV 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
		texCoord.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
		texCoord.y = 1.0f - (float)vertexUV->GetDirectArray().GetAt(index).mData[1];

		// UV 값 반환.
		return texCoord;
	}

	default:
	{
		MessageBox(NULL, L"UV 값이 유효하지 않습니다", L"오류", MB_OK);
		return NULL;
	}
	}

	return NULL;
}

XMFLOAT3 ReadNormal(FbxMesh * mesh, int controlPointIndex, int vertexCounter)
{
	XMFLOAT3 normal(0.0f, 0.0f, 0.0f);

	// 노멀이 있는지 확인.
	if (mesh->GetElementNormalCount() < 1)
	{
		MessageBox(NULL, L"노멀이 없습니다.", L"오류", MB_OK);
		return NULL;
	}

	FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	const bool isUsingIndex
		= vertexNormal->GetReferenceMode() != FbxGeometryElement::eDirect;
	const int indexCount = isUsingIndex ? vertexNormal->GetIndexArray().GetCount() : 0;

	switch (vertexNormal->GetMappingMode())
	{
		// 현재 정점이 제어점 유형인 경우.
	case FbxGeometryElement::eByControlPoint:
	{
		// 현재 노멀 값을 읽어올 인덱스 얻어오기.
		int index = isUsingIndex ? vertexNormal->GetIndexArray().GetAt(controlPointIndex) : controlPointIndex;

		// 노멀 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
		normal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
		normal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
		normal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];

		// 노멀 값 반환.
		return normal;
	}

	case FbxGeometryElement::eByPolygonVertex:
	{
		// 현재 노멀 값을 읽어올 인덱스 얻어오기.
		int index = isUsingIndex ? vertexNormal->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

		// 노멀 값 읽어오기. (DirectX와 FBX의 UV의 V좌표는 서로 반대.)
		normal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
		normal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
		normal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];

		// 노멀 값 반환.
		return normal;
	}

	default:
	{
		MessageBox(NULL, L"노멀 값이 유효하지 않습니다", L"오류", MB_OK);
		return NULL;
	}
	}

	return NULL;
}
HRESULT LoadFBX(const char * fileName, std::vector<Vertex>* pOutVertices, std::vector<DWORD>* pOutIndices)
{
	// fbxManager가 생성되지 않았으면 생성.
	if (fbxManager == NULL)
	{
		// 초기 SDK Manager 생성.
		fbxManager = FbxManager::Create();

		FbxIOSettings* pIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(pIOSettings);
	}

	FbxGeometryConverter gConverter(fbxManager);
	FbxImporter* importer
		= FbxImporter::Create(fbxManager, "");
	FbxScene* fbxScene
		= FbxScene::Create(fbxManager, "");

	// 임포터 초기화.
	if (importer->Initialize(fileName, -1, fbxManager->GetIOSettings()) == false)
	{
		return E_FAIL;
	}

	// 3D 씬 임포트.
	if (importer->Import(fbxScene) == false)
	{
		return E_FAIL;
	}

	// 임포터 해제.
	importer->Destroy();

	// FBX Node 읽기.
	FbxNode* fbxRootNode = fbxScene->GetRootNode();
	if (fbxRootNode == NULL)
		return E_FAIL;

	// 모델링 폴리곤 기본 구조를 삼각면으로 설정.
	gConverter.Triangulate(fbxScene, true);

	int childCount = fbxRootNode->GetChildCount();
	for (int ix = 0; ix < childCount; ++ix)
	{
		// 자식 계층 노드 읽어오기.
		FbxNode* fbxChildNode
			= fbxRootNode->GetChild(ix);

		// 속성이 없는 노드는 패스.
		if (fbxChildNode->GetNodeAttribute() == NULL)
			continue;

		// 노드 속성 타입 확인.
		FbxNodeAttribute::EType arrtType
			= fbxChildNode->GetNodeAttribute()->GetAttributeType();

		// 노드 타입이 메시가 아닌 경우 패스.
		if (arrtType != FbxNodeAttribute::EType::eMesh)
			continue;

		// 메시 정보 읽기.
		FbxMesh* fbxMesh = fbxChildNode->GetMesh();

		// 컨트롤 포인트 배열 얻어오기 (정점).
		FbxVector4* vertices = fbxMesh->GetControlPoints();
		// 폴리곤 수 확인.
		int polyCount = fbxMesh->GetPolygonCount();
		int vertexCounter = 0;

		for (int jx = 0; jx < polyCount; ++jx)
		{
			// 폴리곤 구성 정점 개수 확인.
			int vertexCount = fbxMesh->GetPolygonSize(jx);

			for (int kx = 0; kx < vertexCount; ++kx)
			{
				// 삼각면(폴리곤) 번호, 정점 번호를 통해 정점 읽기.
				int vertexIndex = fbxMesh->GetPolygonVertex(jx, kx);

				// 정점 정보 읽기.
				Vertex vertex;
				vertex.position.x = (float)vertices[vertexIndex].mData[0];
				vertex.position.y = (float)vertices[vertexIndex].mData[1];
				vertex.position.z = (float)vertices[vertexIndex].mData[2];

				// UV 읽기.
				vertex.texCoord = ReadUV(fbxMesh, vertexIndex, vertexCounter);

				// 노멀 읽기.
				vertex.normal = ReadNormal(fbxMesh, vertexIndex, vertexCounter);

				// 정점 배열에 데이터 추가.
				pOutVertices->push_back(vertex);
				// 인덱스 배열에 데이터 추가.
				pOutIndices->push_back(vertexCounter);
				++vertexCounter;
			}
		}
	}


	return S_OK;
}


bool InitInput()
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if (IsError(hr, L"입력 초기화 실패")) 
		return false;

	hr = directInput->CreateDevice(GUID_SysKeyboard, &diKeyboard, NULL);
	if (IsError(hr, L"키보드 초기화 실패")) return false;

	hr = directInput->CreateDevice(GUID_SysMouse, &diMouse, NULL);
	if (IsError(hr, L"마우스 초기화 실패")) return false;

	hr = diKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (IsError(hr, L"키보드 포맷 설정 실패")) return false;

	hr = diMouse->SetDataFormat(&c_dfDIMouse);
	if (IsError(hr, L"마우스 포맷 설정 실패")) return false;

	hr = diKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (IsError(hr, L"키보드 협력 레벨 설정 실패")) return false;

	hr = diMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (IsError(hr, L"마우스 협력 레벨 설정 실패")) return false;

	return true;
}

bool DetectKeyboard(BYTE key)
{
	// 키보드 입력 감지를 위한 배열 선언.
	BYTE keyboardState[256];
	//unsigned char keyboardState[256];

	// 입력 획득.
	diKeyboard->Acquire();

	// 키보드 장치 상태 얻어오기.
	diKeyboard->GetDeviceState(sizeof(keyboardState), (void**)&keyboardState);

	// 키보드 입력 확인.
	if (keyboardState[key] & 0x80) 
		return true;

	return false;
}

bool DetectMouse(DIMOUSESTATE & currentMouseState)
{
	// 입력 획득.
	diMouse->Acquire();

	// 마우스 상태 얻어오기.
	diMouse->GetDeviceState(sizeof(currentMouseState),
		&currentMouseState);

	// 마우스 입력이 발생했는지 확인하는 변수.
	bool retValue = false;

	// 마우스 입력.
	if (currentMouseState.lX != prevMouseState.lX
		|| currentMouseState.lY != prevMouseState.lY)
	{
		retValue = true;
	}

	// 입력 값 저장.
	prevMouseState = currentMouseState;

	return retValue;
}


LRESULT CALLBACK WinProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);

		}
	}
	return 0;
	case WM_DESTROY:
	{
		PostQuitMessage(0);

	}
	return 0;

	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
bool InitWindow()
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = NULL;
	wc.lpszClassName = L"WindowClass";
	wc.lpfnWndProc = WinProc;

	if (!RegisterClassEx(&wc))
		return false;

	hwnd = CreateWindow(L"WindowClass", PROJECT_NAME,
		wndStyle, 0, 0, clientWidth, clientHeight, NULL, NULL,
		hInstance, NULL);

	if (hwnd == NULL)
		return false;

	ShowWindow(hwnd, SW_SHOW);
	return true;
}


bool InitLightCB()
{
	// 라이트 상수 버퍼용 구조체 설정.
	CBLight cbLight;
	ZeroMemory(&cbLight, sizeof(cbLight));
	cbLight.lightPosition = XMVectorSet(500.0f, 500.0f, -500.0f, 1.0f);
	cbLight.cameraPosition = cameraPosition;

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

bool InitDepthStencilBuffer()
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

bool InitDirect3D()
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
void SetAspecRation(UINT width, UINT height)
{
	/*aspectRatio = static_cast<float>(width) /
		static_cast<float>(height);*/
	aspectRatio = (float)width / (float)height;
	modifiedProjection = true;
}
void SetViewport()
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

	SetAspecRation(clientWidth, clientHeight);
}

bool InitScene()
{
	// 셰이더 컴파일.
	HRESULT hr;

	// 정점 셰이더 컴파일해서 정점 셰이더 버퍼에 저장.
	hr = D3DX11CompileFromFile(L"SpecularVS.fx", NULL, NULL,
		"main", "vs_4_0", NULL, NULL, NULL,
		&vertexShaderBuffer, NULL, NULL);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"정점 셰이더 컴파일 실패.", L"오류.", MB_OK);
		return false;
	}

	// 정점 셰이더 생성.
	hr = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"정점 셰이더 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 정점 셰이더 단계에 바인딩(설정, 연결)binding.
	pDeviceContext->VSSetShader(vertexShader, NULL, NULL);

	// 픽셀 셰이더 컴파일.
	hr = D3DX11CompileFromFile(L"SpecularPS.fx", NULL, NULL,
		"main", "ps_4_0", NULL, NULL, NULL, &pixelShaderBuffer,
		NULL, NULL);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"픽셀 셰이더 컴파일 실패.", L"오류.", MB_OK);
		return false;
	}

	// 픽셀 셰이더 생성.
	hr = pDevice->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"픽셀 셰이더 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 픽셀 셰이더 설정.
	pDeviceContext->PSSetShader(pixelShader, NULL, NULL);

	// 모델 로드.
	/*if (LoadModel("cube.txt") == false)
		return false;*/
	hr = LoadFBX("HeroTPP.fbx", &vertices, &indices);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"FBX 로드 실패", L"오류", MB_OK);
		return false;
	}

	// 정점/인덱스 개수 설정.
	nVertices = vertices.size();
	nIndices = indices.size();

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
	hr = pDevice->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"정점 버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 정점 버퍼 바인딩.
	pDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

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
	hr = pDevice->CreateBuffer(&ibDesc, &ibData, &indexBuffer);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"인덱스 버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 인덱스 버퍼 바인딩(binding).
	pDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 입력 레이아웃.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 입력 레이아웃 생성.
	hr = pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &vertexInputLayout);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"입력 레이아웃 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	// 입력 레이아웃 바인딩.
	pDeviceContext->IASetInputLayout(vertexInputLayout);

	// 정점을 이어서 그릴 방식 설정.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 뷰포트 설정.
	SetViewport();

	return true;
}


void UpdateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	modifiedProjection = false;
}

void UpdateViewMatrix()
{
	viewMatrix = XMMatrixLookAtLH(cameraPosition, cameraLook, cameraUp);
}


void SetPosition(XMVECTOR pos)
{
	cameraPosition = pos;
	UpdateViewMatrix();
}
void SetFovY(float value)
{
	fovY = value * Deg2Rad;
	modifiedProjection = true;

}

bool InitTransformation()
{
	// 월드 변환 행렬 설정.
	worldMatrix = XMMatrixIdentity();

	// 회전.
	XMMATRIX rotationX
		= XMMatrixRotationX(XMConvertToRadians(-90.0f));
	XMMATRIX rotationY
		= XMMatrixRotationY(XMConvertToRadians(180.0f));
	XMMATRIX rotation = rotationX * rotationY;
	// 이동.
	XMMATRIX translation = XMMatrixTranslation(0.0f, -90.0f, 0.0f);
	worldMatrix = rotation * translation;

	// 카메라 정보 설정.
	SetPosition(XMVectorSet(0.0f, 0.0f, -200.0f, 0.0f));
	// 뷰 변환 행렬 설정.
	UpdateViewMatrix();

	// 투영 변환 행렬 설정. (90도 -> 라디안).
	SetFovY(75.0f);
	//float fovY = XM_PI / 4.0f;		// 세로 시야각 설정.
	//float fovY = 75.0f * 3.14f / 180.0f;	
	//float fovY = 45.0f * 3.14f / 180.0f;
	UpdateProjectionMatrix();

	// 버퍼에 저장.
	// DX는 행우선 / HLSL는 열우선.

	cbMatrices.world = XMMatrixTranspose(worldMatrix);
	cbMatrices.view = XMMatrixTranspose(viewMatrix);
	cbMatrices.projection = XMMatrixTranspose(projectionMatrix);

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
	cbData.pSysMem = &cbMatrices;

	// 버퍼 생성.
	HRESULT hr = pDevice->CreateBuffer(&cbDesc, &cbData, &cBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"상수 버퍼 생성 실패.", L"오류", MB_OK);
		return false;
	}

	// 바인딩.
	pDeviceContext->VSSetConstantBuffers(0, 1, &cBuffer);

	return true;
}

bool InitTexture()
{
	// 텍스처 파일 로드.
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		pDevice, L"T_Chr_FPS_D.png", NULL, NULL, &pTexture, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"텍스처 로드 실패", L"오류", MB_OK);
		return false;
	}

	// 샘플러 스테이트.
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 샘플러 스테이트 생성.
	hr = pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"샘플러 스테이트 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 텍스처 바인딩.
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	// 샘플러 스테이트 바인딩.
	pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);

	return true;
}

// 초기화 메소드.
bool Init()
{
	// Win32 초기화.
	if (InitWindow() == false)
		return false;

	// Direct3D 초기화.
	if (InitDirect3D() == false)
		return false;

	// 타이머 초기화.
	StartTime();

	// 장면 초기화.
	if (InitScene() == false)
		return false;

	// 공간 변환 행렬 초기화.
	if (InitTransformation() == false)
		return false;

	// 라이트 버퍼 초기화.
	if (InitLightCB() == false)
		return false;

	// 텍스처 초기화.
	if (InitTexture() == false)
		return false;

	// 입력 초기화.
	if (!InitInput())
		return false;

	return true;
}

void ClearBackBuffer()
{
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

}

void Present()
{
	pSwapChain->Present(0, 0);
}

void DrawMesh()
{
	// 갱신된 행렬 값으로 바인딩된 버퍼 업데이트.
	cbMatrices.world = XMMatrixTranspose(worldMatrix);
	cbMatrices.view = XMMatrixTranspose(viewMatrix);
	cbMatrices.projection = XMMatrixTranspose(projectionMatrix);
	pDeviceContext->UpdateSubresource(cBuffer, 0, 0, &cbMatrices, 0, 0);

	pDeviceContext->DrawIndexed(nIndices, 0, 0);

	
}




void GetPlaneDistance(float & zN, float & zF)
{
	zN = nearZ;
	zF = farZ;
}

void SetPlaneDistance(float zN, float zF)
{
	nearZ = zN;
	farZ = zF;
}



void MoveForward(float direction)
{
	cameraPosition += direction * moveSpeed * cameraForward;
}

void MoveRight(float direction)
{
	cameraPosition += direction * moveSpeed * cameraRight;
}

void MoveUp(float direction)
{
	cameraPosition += direction * moveSpeed* cameraUp;
}

void Yaw(float angle)
{
	yaw += angle * rotationSpeed * 0.001f;
}

void Pitch(float angle)
{
	pitch += angle * rotationSpeed * 0.001f;
}





void UpdateCamera()
{
	if (modifiedProjection)
		UpdateProjectionMatrix();

	XMMATRIX rotMatrix
		= XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

	cameraLook
		= XMVector3TransformCoord(defaultForward, rotMatrix);
	cameraLook = XMVector3Normalize(cameraLook);

	XMMATRIX rotYTemp
		= XMMatrixRotationY(yaw);

	cameraRight
		= XMVector3TransformCoord(defaultRight, rotYTemp);
	cameraForward
		= XMVector3TransformCoord(defaultForward, rotYTemp);
	cameraUp
		= XMVector3TransformCoord(defaultUp, rotYTemp);

	cameraLook = cameraPosition + cameraLook;

	UpdateViewMatrix();
}




void ProcessInput(double deltaTime)
{
	if (DetectKeyboard(DIK_ESCAPE))
	{
		PostQuitMessage(0);
		return;
	}

	if (DetectKeyboard(DIK_LEFT) || DetectKeyboard(DIK_A))
	{
		// 카메라 왼쪽 이동.
		MoveRight(-deltaTime);
	}

	if (DetectKeyboard(DIK_RIGHT) || DetectKeyboard(DIK_D))
	{
		MoveRight(deltaTime);
	}

	if (DetectKeyboard(DIK_UP) || DetectKeyboard(DIK_W))
	{
		MoveForward(deltaTime);
	}

	if (DetectKeyboard(DIK_DOWN) || DetectKeyboard(DIK_S))
	{
		MoveForward(-deltaTime);
	}

	if (DetectKeyboard(DIK_Q))
	{
		MoveUp(deltaTime);
	}

	if (DetectKeyboard(DIK_E))
	{
		MoveUp(-deltaTime);
	}

	if (DetectMouse(currentMouseState))
	{
		// 카메라 회전.
		Yaw(currentMouseState.lX * deltaTime);
		Pitch(currentMouseState.lY * deltaTime);
	}
}

void Update(double deltaTime)
{
	UpdateCamera();
}

void Render()
{
	// 색상 설정.
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	//float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// 화면 칠하기.
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// 뎁스/스텐실 뷰 지우기.
	pDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 정점 그리기. (Draw Call. 드로우 콜).
	//pDeviceContext->Draw(nVertices, 0);
	//pDeviceContext->DrawIndexed(nIndices, 0, 0);
	DrawMesh();

	// 스왑체인 교체.
	pSwapChain->Present(1, 0);
}


LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE prevInstance,
	LPSTR lpCmdLine,
	int nCmdLine)
{
	hInstance = hinstance;
	if (!Init())
		return false;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 타이머 업데이트.
			UpdateTimer();
			// 델타 타임(프레임 시간) 계산.
			double deltaTime = DeltaTime();
			ProcessInput(deltaTime);
			// 엔진 업데이트.
			Update(deltaTime);
			Render();		
		}
	}

	return 0;
}




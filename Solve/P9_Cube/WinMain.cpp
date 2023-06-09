#include <Windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

struct Vertex
{
	XMFLOAT3 position;		
	XMFLOAT2 texCoord;		
	XMFLOAT3 normal;		

	Vertex() { }
	Vertex(float x, float y, float z) : position(x, y, z) { }
	Vertex(XMFLOAT3 position) : position(position) { }

	Vertex(XMFLOAT3 position,
		XMFLOAT2 texCoord,
		XMFLOAT3 normal) : position(position), texCoord(texCoord),
		normal(normal) { }
};

struct CBPerObject
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

HWND hwnd;
HINSTANCE hInstance;
UINT clientWidth=800;
UINT clientHeight=600;
LPCWSTR applicationName = PROJECT_NAME;
DWORD wndStyle= WS_OVERLAPPEDWINDOW;

// Device , Context
ID3D11Device* pDevice;
ID3D11DeviceContext* pDeviceContext;
IDXGISwapChain* pSwapChain;
ID3D11RenderTargetView* pRenderTargetView;

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
Vertex* vertices;				

vector< Vertex> arrayVertex;

DWORD* indices;					



// Transform
XMMATRIX worldMatrix;			
XMMATRIX viewMatrix;			
XMMATRIX projectionMatrix;		
XMVECTOR cameraPos;				
XMVECTOR cameraTarget;			
XMVECTOR cameraUp;				
ID3D11Buffer* cBuffer;			

// Texture
ID3D11ShaderResourceView* pTexture;
ID3D11SamplerState* pSamplerState;

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

bool InitDirect3D()
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hwnd;	
	swapDesc.Windowed = true;		
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = clientWidth;
	swapDesc.BufferDesc.Height = clientHeight;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &pSwapChain, &pDevice,
		NULL, &pDeviceContext);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"장치 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	ID3D11Texture2D* pBackBufferTexture;
	hr = pSwapChain->GetBuffer(NULL,
		__uuidof(ID3D11Texture2D),
		(void**)&pBackBufferTexture);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"백버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	hr = pDevice->CreateRenderTargetView(
		pBackBufferTexture, NULL, &pRenderTargetView);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"렌더 타겟 생성 실패.", L"오류.", MB_OK);
		return false;
	}
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	if (pBackBufferTexture)
	{
		pBackBufferTexture->Release();
		pBackBufferTexture = NULL;
	}
	return true;
}



bool LoadModel(const char * fileName)
{
	ifstream fin;
	fin.open(fileName);
	if (fin.fail())
		return false;
	char input;					
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> nVertices;			
	nIndices = nVertices;		
	vertices = new Vertex[nVertices];
	indices = new DWORD[nIndices];
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);
	for (int ix = 0; ix < nVertices; ++ix)
	{
		float x, y, z, u, v, nx, ny, nz;			// 추출할 변수 선언.
		fin >> x >> y >> z;
		fin >> u >> v;
		fin >> nx >> ny >> nz;		
		Vertex temp = Vertex(XMFLOAT3(x, y, z), XMFLOAT2(u, v), XMFLOAT3(nx, ny, nz));
		int index = 0;
		auto it = arrayVertex.begin();
		for (  ; it != arrayVertex.end(); ++it, ++index)
		{
			Vertex& av = *(it);

			if ( ( temp.position.x == av.position.x && temp.position.y == av.position.y && temp.position.z == av.position.z))
			{				
				break;
			}		
		}	
		if (it == arrayVertex.end())
		{
			arrayVertex.insert(arrayVertex.end(), temp);
			indices[ix] = arrayVertex.size() - 1;
		}
		else
		{		
			indices[ix] = index;
		}
		
	}
	fin.close();
	return true;
}

bool InitScene()
{
	HRESULT hr;
	hr = D3DX11CompileFromFile(L"EffectVS.fx", NULL, NULL,
		"main", "vs_4_0", NULL, NULL, NULL,
		&vertexShaderBuffer, NULL, NULL);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"정점 셰이더 컴파일 실패.", L"오류.", MB_OK);
		return false;
	}

	hr = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"정점 셰이더 생성 실패.", L"오류.", MB_OK);
		return false;
	}
	
	pDeviceContext->VSSetShader(vertexShader, NULL, NULL);

	hr = D3DX11CompileFromFile(L"EffectPS.fx", NULL, NULL,
		"main", "ps_4_0", NULL, NULL, NULL, &pixelShaderBuffer,
		NULL, NULL);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"픽셀 셰이더 컴파일 실패.", L"오류.", MB_OK);
		return false;
	}

	hr = pDevice->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);

	if (FAILED(hr))
	{
		MessageBox(hwnd, L"픽셀 셰이더 생성 실패.", L"오류.", MB_OK);
		return false;
	}
	
	pDeviceContext->PSSetShader(pixelShader, NULL, NULL);
	if (LoadModel("cube.txt") == false)
		return false;

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));

	vbDesc.ByteWidth = sizeof(Vertex) * arrayVertex.size();
	vbDesc.CPUAccessFlags = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.MiscFlags = 0;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vbData;
	ZeroMemory(&vbData, sizeof(vbData));
	vbData.pSysMem = &arrayVertex[0];

	hr = pDevice->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"정점 버퍼 생성 실패.", L"오류.", MB_OK);
		return false;
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

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
	ibData.pSysMem = indices;

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
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = clientWidth;
	viewport.Height = clientHeight;

	// 뷰포트 설정.
	pDeviceContext->RSSetViewports(1, &viewport);

	return true;
}
bool InitTransformation()
{
	// 월드 변환 행렬 설정.
	worldMatrix = XMMatrixIdentity();

	// 카메라 정보 설정.
	cameraPos = XMVectorSet(4.0f, 4.0f, -8.0f, 0.0f);
	cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// 뷰 변환 행렬 설정.
	viewMatrix = XMMatrixLookAtLH(cameraPos, cameraTarget, cameraUp);

	// 투영 변환 행렬 설정. (90도 -> 라디안).
	float fovY = XM_PI / 4.0f;		// 세로 시야각 설정.
	//float fovY = 45.0f * 3.14f / 180.0f;
	projectionMatrix = XMMatrixPerspectiveFovLH(fovY, (float)clientWidth / (float)clientHeight, 1.0f, 1000.0f);

	// 버퍼에 저장.
	// DX는 행우선 / HLSL는 열우선.
	CBPerObject cData;
	ZeroMemory(&cData, sizeof(CBPerObject));
	// XMMatrixTranspose->전치행렬 구하는 API.
	cData.world = XMMatrixTranspose(worldMatrix);
	cData.view = XMMatrixTranspose(viewMatrix);
	cData.projection = XMMatrixTranspose(projectionMatrix);

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
	cbData.pSysMem = &cData;

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
		pDevice, L"iron.jpg", NULL, NULL, &pTexture, NULL);
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
	pDeviceContext->DrawIndexed(nIndices, 0, 0);
}


LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE prevInstance,
	LPSTR lpCmdLine,
	int nCmdLine)
{
	hinstance = hinstance;
	if (InitWindow() == false)
		return -1;

	if (InitDirect3D() == false)
		return -1;

	if (InitScene() == false)
		return -1;

	if (InitTransformation() == false)
		return -1;

	if (InitTexture() == false)
		return -1;

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
			ClearBackBuffer();
			DrawMesh();
			Present();
		}
	}

	return 0;
}




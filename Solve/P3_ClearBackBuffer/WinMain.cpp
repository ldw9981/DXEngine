#include <Windows.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

HWND hwnd;
HINSTANCE hInstance;
UINT clientWidth=800;
UINT clientHeight=600;
LPCWSTR applicationName = PROJECT_NAME;
DWORD wndStyle= WS_OVERLAPPEDWINDOW;

ID3D11Device* pDevice;
ID3D11DeviceContext* pDeviceContext;
IDXGISwapChain* pSwapChain;
ID3D11RenderTargetView* pRenderTargetView;


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

void ClearBackBuffer()
{
	float color[4] = { 1.0f, 0.0f, 1.0f, 0.0f };
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);
	
}

void Present()
{
	pSwapChain->Present(0, 0);
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
			Present();
		}
	}

	return 0;
}




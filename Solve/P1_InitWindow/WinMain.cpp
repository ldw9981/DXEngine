#include <Windows.h>

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd;
HINSTANCE hInstance;
UINT clientWidth = 800;
UINT clientHeight = 600;
LPCWSTR applicationName = PROJECT_NAME;
DWORD wndStyle = WS_OVERLAPPEDWINDOW;

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
int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE prevInstance,
	LPSTR lpCmdLine,
	int nCmdLine)
{
	if (InitWindow() == false)
		return -1;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
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

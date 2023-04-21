#include <Windows.h>

// 윈도우 프로시저 함수.
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

// 메인 함수.
int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE prevInstance,
	LPSTR lpCmdLine,
	int nCmdLine)
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
	wc.lpszClassName = NULL;
	wc.lpszClassName = L"WindowClass";
	wc.lpfnWndProc = WinProc;

	// 클래스 등록.
	if (!RegisterClassEx(&wc))
		return 0;

	// 윈도우 생성.
	HWND hwnd;
	hwnd = CreateWindow(L"WindowClass", L"Engine00 - Win Setup",
		WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, 
		hinstance, NULL);

	// 예외 처리.
	if (hwnd == NULL) 
		return 0;

	// 창 띄우기.
	ShowWindow(hwnd, SW_SHOW);

	// 메시지 처리 루프.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 루프 실행.
	while (msg.message != WM_QUIT)
	{
		// 메시지 매핑.
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
				return 0;
			}
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
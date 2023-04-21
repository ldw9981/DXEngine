#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "DXUtil.h"
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class InputClass
{
public:
	InputClass();
	InputClass(HINSTANCE hinstance, HWND hwnd);
	~InputClass();

	bool InitInput();
	bool DetectKeyboard(BYTE key);
	bool DetectMouse(DIMOUSESTATE& currentMouseState);

private:

	IDirectInputDevice8* diKeyboard;
	IDirectInputDevice8* diMouse;

	DIMOUSESTATE prevMouseState;
	//IDirectInput8* directInput;
	LPDIRECTINPUT8 directInput;

	HINSTANCE hinstance;
	HWND hwnd;
};
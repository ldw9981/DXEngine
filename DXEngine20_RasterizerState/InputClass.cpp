#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(HINSTANCE hinstance, HWND hwnd)
{
	this->hinstance = hinstance;
	this->hwnd = hwnd;
}

InputClass::~InputClass()
{
}

bool InputClass::InitInput()
{
	HRESULT hr;
	hr = DirectInput8Create(	hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
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

	hr = diKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (IsError(hr, L"키보드 협력 레벨 설정 실패")) return false;

	hr = diMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY);
	if (IsError(hr, L"마우스 협력 레벨 설정 실패")) return false;

	return true;
}

bool InputClass::DetectKeyboard(BYTE key)
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

bool InputClass::DetectMouse(DIMOUSESTATE & currentMouseState)
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

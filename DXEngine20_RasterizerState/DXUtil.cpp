#include "DXUtil.h"

bool IsError(HRESULT hr, LPCWSTR errorMsg)
{
	if (hr < 0)
	{
		MessageBox(NULL, errorMsg, L"¿À·ù", MB_OK);
		return true;
	}

	return false;
}

float Rad2Deg = 180.0f / XM_PI;
float Deg2Rad = XM_PI / 180.0f;
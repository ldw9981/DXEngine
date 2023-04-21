#include "ShaderUtil.h"

namespace ShaderUtil
{
	bool CompileShader(LPCWSTR fileName, LPCSTR entry, LPCSTR profile, ID3DBlob ** pOutShaderBuffer)
	{
		HRESULT hr
			= D3DX11CompileFromFile(
				fileName, NULL, NULL, 
				entry, profile, 0, 0, NULL, 
				pOutShaderBuffer, NULL, NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"셰이더 컴파일 오류", L"오류", MB_OK);

			OutputDebugString(L"셰이더 컴파일 오류");
			OutputDebugString(fileName);
			OutputDebugString(L"\n");

			return false;
		}

		return true;
	}

	bool CreateVertexShader(ID3D11Device * device, const void * pointer, SIZE_T size, ID3D11ClassLinkage * linkage, ID3D11VertexShader ** ppVertexShader)
	{
		HRESULT hr = device->CreateVertexShader(pointer, size, linkage, ppVertexShader);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"정점 셰이더 생성 오류", L"오류", MB_OK);

			return false;
		}

		return true;
	}

	void BindVertexShader(ID3D11DeviceContext * deviceContext, 
		ID3D11VertexShader * pVertexShader, ID3D11ClassInstance * const * ppClassInstances, 
		UINT NumClassInstances)
	{
		deviceContext->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
	}

	bool CreatePixelShader(ID3D11Device * device, const void * pointer, SIZE_T size, ID3D11ClassLinkage * linkage, ID3D11PixelShader ** ppPixelShader)
	{
		HRESULT hr = device->CreatePixelShader(pointer, size, linkage, ppPixelShader);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"픽셀 셰이더 생성 오류", L"오류", MB_OK);

			return false;
		}

		return true;
	}

	void BindPixelShader(ID3D11DeviceContext * deviceContext, ID3D11PixelShader * pPixelShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
	{
		deviceContext->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
	}
}
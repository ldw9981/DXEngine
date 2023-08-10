#include "ShaderUtil.h"

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

namespace ShaderUtil
{
	bool CompileShader(LPCWSTR fileName, LPCSTR entry, LPCSTR profile, ID3DBlob ** pOutShaderBuffer)
	{
		HRESULT hr;
		ID3D10Blob* errorMessage = nullptr;	 // ���� �޽����� ������ ����.

		// ���� ���̴� �������ؼ� ���� ���̴� ���ۿ� ����.
		hr = D3DCompileFromFile(fileName, NULL, NULL,
			entry, profile, NULL, NULL,
			pOutShaderBuffer, &errorMessage);
				
		if (FAILED(hr))
		{
			MessageBox(NULL, L"���̴� ������ ����", L"����", MB_OK);

			OutputDebugString(L"���̴� ������ ����");
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
			MessageBox(NULL, L"���� ���̴� ���� ����", L"����", MB_OK);

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
			MessageBox(NULL, L"�ȼ� ���̴� ���� ����", L"����", MB_OK);

			return false;
		}

		return true;
	}

	void BindPixelShader(ID3D11DeviceContext * deviceContext, ID3D11PixelShader * pPixelShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
	{
		deviceContext->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
	}
}
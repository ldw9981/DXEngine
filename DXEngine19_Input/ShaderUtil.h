#pragma once

#include "DXUtil.h"

namespace ShaderUtil
{
	//class Shader;

	// 컴파일.
	//bool CompilShader(Shader shader);
	bool CompileShader(LPCWSTR fileName, LPCSTR entry, LPCSTR profile, ID3DBlob** pOutShaderBuffer);

	// 버텍스 셰이더 생성.
	bool CreateVertexShader(
		ID3D11Device* device, 
		const void* pointer, 
		SIZE_T size, 
		ID3D11ClassLinkage* linkage, 
		ID3D11VertexShader** ppVertexShader);

	// 버텍스 셰이더 바인딩.
	void BindVertexShader(
		ID3D11DeviceContext* deviceContext,
		ID3D11VertexShader *pVertexShader,
		ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances);

	// 픽셀 셰이더 생성.
	bool CreatePixelShader(
		ID3D11Device* device,
		const void* pointer,
		SIZE_T size,
		ID3D11ClassLinkage* linkage,
		ID3D11PixelShader** ppPixelShader);

	// 픽셀 셰이더 바인딩.
	void BindPixelShader(
		ID3D11DeviceContext* deviceContext,
		ID3D11PixelShader *pPixelShader,
		ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances
	);
};


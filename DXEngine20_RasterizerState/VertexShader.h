#pragma once

#include "Shader.h"

class VertexShader : public Shader
{
public:
	VertexShader();
	VertexShader(LPCWSTR fileName);
	VertexShader(LPCWSTR fileName, LPCSTR entry, LPCSTR profile);
	~VertexShader();

	CBPerObject GetWVPMaticex() const { return cbMatrices; }
	void SetWVPMatrices(
		XMMATRIX world,
		XMMATRIX view,
		XMMATRIX projection);

	CBLight GetLightInfo() const { return cbLight; }
	void SetLightInfo(
		XMVECTOR lightPos, 
		XMVECTOR cameraPos);

	virtual bool CompileShader() override;
	virtual bool CreateShader(ID3D11Device* device) override;
	virtual void BindShader(ID3D11DeviceContext* deviceContext) override;
	virtual void Release() override;

	// 래스터 라이저 스테이트 설정
	bool CreateRasterizerState(ID3D11Device* device, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);
	void BindRasterizerState(ID3D11DeviceContext* deviceContext);
private:

	ID3D11VertexShader* vertexShader;
	CBPerObject cbMatrices;
	CBLight cbLight;
	ID3D11RasterizerState* rsState;
};
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

private:

	ID3D11VertexShader* vertexShader;
	CBPerObject cbMatrices;
	CBLight cbLight;
};
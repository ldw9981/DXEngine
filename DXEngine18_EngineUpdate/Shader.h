#pragma once

#include "DXUtil.h"
#include "ShaderUtil.h"

class Shader
{
public:
	Shader();
	Shader(LPCWSTR fileName);
	Shader(LPCWSTR fileName, LPCSTR entry, LPCSTR profile);
	virtual ~Shader();

protected:

	// ¼ÎÀÌ´õ º¯¼ö.
	LPCWSTR fileName;
	LPCSTR entryPoint;
	LPCSTR profile;
	ID3DBlob* shaderBuffer;

protected:

	virtual bool CompileShader() = 0;
	virtual bool CreateShader(ID3D11Device* device) = 0;
	virtual void BindShader(ID3D11DeviceContext* deviceContext) = 0;
	virtual void Release();

public:

	void SetFileName(LPCWSTR fileName)
	{ 
		this->fileName = fileName; 
	}
	LPCWSTR GetFileName() const { return fileName; }
	LPCSTR GetEntry() const { return entryPoint; }
	LPCSTR GetProfile() const { return profile; }
	ID3DBlob* GetShaderBuffer() const { return shaderBuffer; }
};
#pragma once

#include "Shader.h"
#include <vector>

class PixelShader : public Shader
{
public:
	PixelShader();
	PixelShader(LPCWSTR fileName);
	PixelShader(LPCWSTR fileName, LPCSTR entry, LPCSTR profile);
	~PixelShader();

	virtual bool CompileShader() override;
	virtual bool CreateShader(ID3D11Device* device) override;
	virtual void BindShader(ID3D11DeviceContext* deviceContext) override;
	virtual void Release() override;

	bool CreateSamplerState(ID3D11Device* device);
	void BindSamplerState(
		ID3D11DeviceContext* deviceContext);

	bool LoadTextures(ID3D11Device* device);
	void BindTextures(ID3D11DeviceContext* deviceContext);

	// Getter / Setter.
	ID3D11PixelShader* GetPixelShader() const { return pixelShader; }
	std::vector<Texture> GetTextureArray() const { return textures; }
	void AddTexture(Texture newTexture) {  textures.push_back(newTexture);  }
	ID3D11SamplerState* GetSamplerState() const { return samplerState; }

private:

	ID3D11PixelShader* pixelShader;
	// ≈ÿΩ∫√≥.
	std::vector<Texture> textures;
	ID3D11SamplerState* samplerState;
};
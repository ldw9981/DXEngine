#include "Engine.h"

Engine::Engine(HINSTANCE hinstance) 
	: DXApp(hinstance)
{

}

Engine::~Engine()
{

}

bool Engine::Init()
{
	if (DXApp::Init() == false) 
		return false;

	return true;
}

void Engine::Update()
{
	SetUVTime();
}

void Engine::Render()
{
	// 색상 설정.
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	// 화면 칠하기.
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// 정점 그리기. (Draw Call. 드로우 콜).
	//pDeviceContext->Draw(nVertices, 0);
	pDeviceContext->DrawIndexed(nIndices, 0, 0);

	// 스왑체인 교체.
	pSwapChain->Present(0, 0);
}

void Engine::SetUVTime()
{
	CBPerFrame cData;
	ZeroMemory(&cData, sizeof(cData));
	cData.time.x = GetTickCount() / 1000.0f;
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(CBPerFrame);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA timeData;
	ZeroMemory(&timeData, sizeof(timeData));
	timeData.pSysMem = &cData;

	ID3D11Buffer* cBufferFrame;
	pDevice->CreateBuffer(&cbDesc, &timeData, &cBufferFrame);

	pDeviceContext->VSSetConstantBuffers(1, 1, &cBufferFrame);
}

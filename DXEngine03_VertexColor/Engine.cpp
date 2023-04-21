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

}

void Engine::Render()
{
	// 색상 설정.
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

	// 화면 칠하기.
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// 정점 그리기.
	pDeviceContext->Draw(nVertices, 0);

	// 스왑체인 교체.
	pSwapChain->Present(0, 0);
}
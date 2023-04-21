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

void Engine::Update(double deltaTime)
{
	RotateObject(deltaTime);
}

void Engine::Render(double deltaTime)
{
	// 색상 설정.
	/*float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };*/
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// 화면 칠하기.
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// 정점 그리기. (Draw Call. 드로우 콜).
	//pDeviceContext->Draw(nVertices, 0);
	pDeviceContext->DrawIndexed(nIndices, 0, 0);

	// 스왑체인 교체.
	pSwapChain->Present(0, 0);
}

float rotX = 0.01f;
float rotY = 0.01f;
float rotZ = 0.01f;

float tranZ = 0.1f;

float doublePI = XM_PI * 2.0f; // 360degree
void Engine::RotateObject(double deltaTime)
{
	/*
	rotZ += XMConvertToRadians(0.01f);
	rotY += XMConvertToRadians(0.01f);
	rotZ += XMConvertToRadians(0.01f);	
	*/
	rotZ += XMConvertToRadians(0.01f*deltaTime);
	rotY += XMConvertToRadians(30.0f*deltaTime);
	rotZ += XMConvertToRadians(0.01f*deltaTime);
	if (rotX >= doublePI)
		rotX = 0.0f;

	if (rotY >= doublePI)
		rotY = 0.0f;

	if (rotZ >= doublePI)
		rotZ = 0.0f;

	/*
	tranZ += 0.01f;
	
	if (tranZ >= 50.0f)
		tranZ = 0.0f;
	*/
	
	XMMATRIX rotationX = XMMatrixRotationX(0.0f);
	XMMATRIX rotationY = XMMatrixRotationY(rotY);
	XMMATRIX rotationZ = XMMatrixRotationZ(0.0f);

	//이동행렬
	XMMATRIX scale = XMMatrixIdentity();
	XMMATRIX rotation = rotationX * rotationY * rotationZ;
	XMMATRIX translation = XMMatrixTranslation(0.0f,0.0f, 10.0f);

	worldMatrix = XMMatrixIdentity();
	worldMatrix = scale * rotation * translation;
	//
	CBPerObject cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.world = XMMatrixTranspose(worldMatrix);
	cbData.view = XMMatrixTranspose(viewMatrix);
	cbData.projection = XMMatrixTranspose(projectionMatrix);

	// 데이터 갱신
	pDeviceContext->UpdateSubresource(cBuffer, 0, 0, &cbData, 0, 0);
}

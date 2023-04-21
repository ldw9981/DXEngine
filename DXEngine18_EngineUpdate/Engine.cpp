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
	RotateObject(deltaTime);				// 물체 회전.
}

void Engine::Render()
{
	// 색상 설정.
	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	//float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// 화면 칠하기.
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// 뎁스/스텐실 뷰 지우기.
	pDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 정점 그리기. (Draw Call. 드로우 콜).
	//pDeviceContext->Draw(nVertices, 0);
	//pDeviceContext->DrawIndexed(nIndices, 0, 0);
	DrawMesh();

	// 스왑체인 교체.
	pSwapChain->Present(1, 0);
}

void Engine::DrawMesh()
{
	for (int ix = 0; ix < meshes.size(); ++ix)
	{
		Mesh* mesh = &meshes[ix];

		BindShader(mesh);
		BindVertexBuffer(mesh);
		BindIndexBuffer(mesh);
		BindInputLayout(mesh);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mesh->BindTextures(pDeviceContext);
		mesh->BindSamplerState(pDeviceContext);

		UpdateWVPBuffer(mesh);

		UpdateLightCB(mesh);
		mesh->DrawMesh(pDeviceContext);
	}
}

float rotY = 0.01f;		// 회전 각도 저장용.
float rotX = 0.01f;		// 회전 각도 저장용.
float rotZ = 0.01f;		// 회전 각도 저장용.
//float pi = 3.141592f;
float doublePI = XM_PI * 2.0f;
void Engine::RotateObject(double deltaTime)
{
	// 프레임 마다 0.01도씩 누적.
	/*rotY += XMConvertToRadians(0.01f);
	rotX += XMConvertToRadians(0.01f);
	rotZ += XMConvertToRadians(0.01f);*/
	rotY += XMConvertToRadians(30.0f * deltaTime);
	rotX += XMConvertToRadians(30.0f * deltaTime);
	rotZ += XMConvertToRadians(30.0f * deltaTime);
	if (rotY >= doublePI)		// 한바퀴 돌면 0도로 설정.
		rotY = 0.0f;
	if (rotX >= doublePI)
		rotX = 0.0f;
	if (rotZ >= doublePI)
		rotZ = 0.0f;

	// 회전 행렬.
	XMMATRIX rotationX = XMMatrixRotationX(0.0f);
	XMMATRIX rotationY = XMMatrixRotationY(rotY);
	XMMATRIX rotationZ = XMMatrixRotationZ(0.0f);
	XMMATRIX rotation = rotationZ * rotationX * rotationY;

	// 이동 행렬.
	XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	// 스케일(Scale 비례).
	XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	// 월드 행렬 갱신.
	worldMatrix = XMMatrixIdentity();
	worldMatrix = scale * rotation * translation;

	// 셰이더 상수버퍼에 전달할 데이터 설정.
	CBPerObject cbData;
	ZeroMemory(&cbData, sizeof(cbData));
	cbData.world = XMMatrixTranspose(worldMatrix);
	cbData.view = XMMatrixTranspose(viewMatrix);
	cbData.projection = XMMatrixTranspose(projectionMatrix);

	// 데이터 갱신.
	pDeviceContext->UpdateSubresource(cBuffer, 0, 0, &cbData, 0, 0);
}

#pragma once

#include "DXUtil.h"

class CameraClass
{
public:
	CameraClass();
	~CameraClass();

	float GetFovY() const { return fovY * Rad2Deg; }
	void SetFovY(float fovY);

	float GetAspecRation() const { return aspectRatio; }
	void SetAspecRation(UINT width, UINT height);

	void GetPlaneDistance(float& zN, float& zF);
	void SetPlaneDistance(float zN, float zF);

	XMMATRIX GetViewMatrix() const { return viewMatrix; }
	XMMATRIX GetProjectionMatrix() const { return projectionMatrix; }

	XMVECTOR GetPosition() const { return cameraPosition; }
	void SetPosition(XMVECTOR pos);

	XMVECTOR GetRight() const { return cameraRight; }
	XMVECTOR GetLook() const { return cameraLook; }
	XMVECTOR GetUp() const { return cameraUp; }

	void MoveForward(float dist);
	void MoveRight(float dist);
	void MoveUp(float dist);

	void Yaw(float angle);
	void Pitch(float angle);

	void UpdateCamera();

private:

	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

private:

	float fovY;				//시야각.
	float aspectRatio;		// 화면 종횡비.
	float nearZ;				// 근평면 거리.
	float farZ;				// 원평면 거리.

	float yaw = 0.0f;		// yaw 회전 값.
	float pitch = 0.0f;		// pitch 회전 값.

	float moveSpeed = 200.0f;		// 카메라 이동 속도.
	float rotationSpeed = 30.0f;		// 카메라 회전 속도.

	bool modifiedProjection = false;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	XMVECTOR cameraPosition;
	XMVECTOR cameraLook 
		= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR cameraRight 
		= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR cameraForward
		= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMVECTOR defaultRight 
		= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR defaultUp
		= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR defaultForward
		= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
};
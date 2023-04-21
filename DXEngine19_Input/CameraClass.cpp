#include "CameraClass.h"



CameraClass::CameraClass()
{
	SetFovY(75.0f);
	nearZ = 1.0f;
	farZ = 1000.0f;
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetFovY(float fovY)
{
	this->fovY = fovY * Deg2Rad;
	modifiedProjection = true;
	
}

void CameraClass::SetAspecRation(UINT width, UINT height)
{
	/*aspectRatio = static_cast<float>(width) /
		static_cast<float>(height);*/
	this->aspectRatio = (float)width / (float)height;
	modifiedProjection = true;
}

void CameraClass::GetPlaneDistance(float & zN, float & zF)
{
	zN = nearZ;
	zF = farZ;
}

void CameraClass::SetPlaneDistance(float zN, float zF)
{
	nearZ = zN;
	farZ = zF;
}

void CameraClass::SetPosition(XMVECTOR pos)
{
	cameraPosition = pos;
	UpdateViewMatrix();
}

void CameraClass::MoveForward(float direction)
{
	cameraPosition += direction * moveSpeed * cameraForward;
}

void CameraClass::MoveRight(float direction)
{
	cameraPosition += direction * moveSpeed * cameraRight;
}

void CameraClass::MoveUp(float direction)
{
	cameraPosition += direction * moveSpeed* cameraUp;
}

void CameraClass::Yaw(float angle)
{
	yaw += angle * rotationSpeed * 0.001f;
}

void CameraClass::Pitch(float angle)
{
	pitch += angle * rotationSpeed * 0.001f;
}

void CameraClass::UpdateCamera()
{
	if (modifiedProjection)
		UpdateProjectionMatrix();

	XMMATRIX rotMatrix
		= XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

	cameraLook
		= XMVector3TransformCoord(defaultForward, rotMatrix);
	cameraLook = XMVector3Normalize(cameraLook);

	XMMATRIX rotYTemp
		= XMMatrixRotationY(yaw);

	cameraRight
		= XMVector3TransformCoord(defaultRight, rotYTemp);
	cameraForward 
		= XMVector3TransformCoord(defaultForward, rotYTemp);
	cameraUp
		= XMVector3TransformCoord(defaultUp, rotYTemp);

	cameraLook = cameraPosition + cameraLook;

	UpdateViewMatrix();
}

void CameraClass::UpdateViewMatrix()
{
	viewMatrix = XMMatrixLookAtLH(cameraPosition, cameraLook, cameraUp);
}

void CameraClass::UpdateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	modifiedProjection = false;
}

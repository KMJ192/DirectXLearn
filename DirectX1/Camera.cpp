#include "Stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	_PositionX = 0.0f;
	_PositionY = 0.0f;
	_PositionZ = 0.0f;
	_RotationX = 0.0f;
	_RotationY = 0.0f;
	_RotationZ = 0.0f;
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	_PositionX = x;
	_PositionY = y;
	_PositionZ = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	_RotationX = x;
	_RotationY = y;
	_RotationZ = z;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(_PositionX, _PositionY, _PositionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(_RotationX, _RotationY, _RotationZ);
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = _ViewMatrix;
}

void Camera::Render()
{
	XMFLOAT3 up;
	XMFLOAT3 position;
	XMFLOAT3 lookAt;
	float yaw;
	float pitch;
	float roll;
	XMMATRIX rotationMatrix;

	//위쪽을 가리키는 벡터를 설정
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//World에서 Camara위치 설정
	position.x = _PositionX;
	position.y = _PositionY;
	position.z = _PositionZ;

	//World에서 Camara 보는 방향 설정
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//yaw(y축), pitch(x축), roll(z축) 회전을 라디안 단위로 설정
	pitch = _RotationX * 0.0174532925f;
	yaw = _RotationX * 0.0174532925f;
	roll = _RotationX * 0.0174532925f;

	//yaw, pitch, roll값에서 회전 행렬 생성
	rotationMatrix = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);

	//lookAt 및 up 벡터를 회전 행렬로 변환하여 뷰가 원점에서 회전
	XMVECTOR LOOKat, UP, POSItion;
	LOOKat = XMVector3TransformCoord(XMLoadFloat3(&lookAt), rotationMatrix);
	UP = XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix);
	POSItion = XMLoadFloat3(&position);

	//회전 된 카메라 위치를 뷰어 위치로 변환.
	LOOKat = XMVectorAdd(POSItion, LOOKat);

	//마지막으로 업데이트 된 세 벡터로 부터 뷰 행렬 생성
	_ViewMatrix = XMMatrixLookAtLH(POSItion, LOOKat, UP);
}

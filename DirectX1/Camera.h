#pragma once
class Camera
{
	float _PositionX;
	float _PositionY;
	float _PositionZ;
	float _RotationX;
	float _RotationY;
	float _RotationZ;
	XMMATRIX _ViewMatrix;

public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);
};
#pragma once

class Camera
{
public:
	Vector3D position, Target;
	Vector3D Forward, Right, Up;

	float WorldPhi, WorldTheta;

	Camera(Vector3D pos, Vector3D lookDir)
	{
		position = pos;
		Forward = lookDir.Normalized();
		Up = Vector3D(0, 1, 0);
		Right = Vector3D::Cross(Up, Forward);
		Target = position + Forward;
		WorldPhi = 0;
		WorldTheta = 0;
	}

	Vector3D& GetUp() { return this->Up; }
	void SetForward(Vector3D value)
	{
		this->Forward = value;
		this->Right = Vector3D::Cross(Up, Forward);
	}
	void SetRight(Vector3D& value)
	{
		this->Right = value;
		this->Forward = Vector3D::Cross(Right, Up);
	}

	void RotateY(float yRad)
	{
		this->Forward = Mat3x3::MultiplyVectorByMatrix3(this->Forward, Mat3x3::RotationY(yRad), true).Normalized();
		this->Up = Mat3x3::MultiplyVectorByMatrix3(this->Up, Mat3x3::RotationY(yRad), true).Normalized();
		this->Right = Vector3D::Cross(Up, Forward);
		this->Right.y = 0;
	}
	void RotateX(float xRad)
	{
		this->Right.y = 0;
		this->Forward = Mat3x3::MultiplyVectorByMatrix3(this->Forward, Mat3x3::Rotation(this->Right, xRad), true).Normalized();
		this->Up = Vector3D::Cross(Forward, Right);
	}
};
	

#pragma once

class Camera
{
public:
	Vector3D position, Target;
	Vector3D Forward, Right, Up, forwardOnXZ;

	Camera(Vector3D pos, Vector3D lookDir)
	{
		position = pos;
		Forward = lookDir.Normalized();
		forwardOnXZ = Vector3D(Forward.x, 0, Forward.z);
		Up = Vector3D(0, 1, 0);
		Right = Vector3D::Cross(Up, Forward);
		Target = position + Forward;
	}

	Vector3D& GetUp() { return this->Up; }

	void SetPosition(float x, float y, float z)
	{
		this->position.x = x;
		this->position.y = y;
		this->position.z = z;
		this->Target = this->position + this->Forward;
	}
	void SetPosition(Vector3D newPosition)
	{
		this->position.x = newPosition.x;
		this->position.y = newPosition.y;
		this->position.z = newPosition.z;
		this->Target = this->position + this->Forward;
	}
	Vector3D GetPosition() { return this->position; }

	void SetForward(Vector3D value)
	{
		this->Forward = value;
		this->forwardOnXZ.x = Forward.x;
		this->forwardOnXZ.z = Forward.z;
		this->Right = Vector3D::Cross(Up, Forward);
	}
	void SetRight(Vector3D& value)
	{
		this->Right = value;
		this->Forward = Vector3D::Cross(Right, Up);
		this->forwardOnXZ.x = Forward.x;
		this->forwardOnXZ.z = Forward.z;
	}

	void RotateY(float yRad)
	{
		this->Forward = Mat3x3::MultiplyVectorByMatrix3(this->Forward, Mat3x3::RotationY(yRad), true).Normalized();
		this->forwardOnXZ = Mat3x3::MultiplyVectorByMatrix3(this->forwardOnXZ, Mat3x3::RotationY(yRad), true).Normalized();
		this->Up = Mat3x3::MultiplyVectorByMatrix3(this->Up, Mat3x3::RotationY(yRad), true).Normalized();
		this->Right = Vector3D::Cross(Up, Forward);
		this->Right.y = 0;
		this->Target = this->position + this->Forward;
	}
	void RotateX(float xRad)
	{
		this->Right.y = 0;
		this->Forward = Mat3x3::MultiplyVectorByMatrix3(this->Forward, Mat3x3::Rotation(this->Right, xRad), true).Normalized();
		this->Up = Vector3D::Cross(Forward, Right);
		this->Target = this->position + this->Forward;
	}
};
	

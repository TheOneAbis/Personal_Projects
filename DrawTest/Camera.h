#pragma once

class Camera
{
public:
	Vector3D position, Target;
	Vector3D Forward, Right, Up, forwardOnXZ;

	float WorldPhi, WorldTheta;

	Camera(Vector3D pos, Vector3D lookDir)
	{
		position = pos;
		Forward = lookDir.Normalized();
		forwardOnXZ = Vector3D(Forward.x, 0, Forward.z);
		Up = Vector3D(0, 1, 0);
		Right = Vector3D::Cross(Up, Forward);
		Target = position + Forward;

		this->WorldPhi = this->Forward.y > 0 ? acos(this->Forward.Dot(Vector3D(this->Forward.x, 0, this->Forward.z).Normalized())) :
			-acos(this->Forward.Dot(Vector3D(this->Forward.x, 0, this->Forward.z).Normalized()));
		cout << WorldPhi << endl;

		this->WorldTheta = this->Forward.x > 0 ? acos(Vector3D(this->Forward.x, 0, this->Forward.z).Normalized().Dot(GlobalForward())) :
			-acos(Vector3D(this->Forward.x, 0, this->Forward.z).Normalized().Dot(GlobalForward()));
	}

	Vector3D& GetUp() { return this->Up; }

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
	}
	void RotateX(float xRad)
	{
		this->Right.y = 0;
		this->Forward = Mat3x3::MultiplyVectorByMatrix3(this->Forward, Mat3x3::Rotation(this->Right, xRad), true).Normalized();
		this->Up = Vector3D::Cross(Forward, Right);
	}
};
	

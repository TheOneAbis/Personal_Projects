#pragma once
#include "Vector3D.h"
#define arrLen3x3 3
#define arrLen4x4 4


struct Mat3x3
{
private:
    float m[3][3] = { 0 };
public:

    static float** Identity();

    static float** Empty();

    // Rotation X Matrix
    static float** RotationX(float theta);
    // Rotation Y Matrix
    static float** RotationY(float theta);
    // Rotation Z Matrix
    static float** RotationZ(float theta);
    // Rotation about any vector
    static float** Rotation(Vector3D axis, float theta);

    // Multiply a Vector3 by a 3 X 3 Matrix, returns the new altered Vector3
    static Vector3D MultiplyVectorByMatrix3(const Vector3D& v, float** matrix3x3, bool del);

private:
    static float** MakeNewArr();
};


struct Mat4x4
{
    
public:
    float m[4][4] = { 0 };

    // Projection Matrix
    static float** Projection(float fNear, float fFar, float fov, float aspectRatio);
    
    // Point At Matrix (Point camera at object)
    static float** PointAt(Vector3D& pos, Vector3D& target, Vector3D& up);

    // Only works for rotation/translation matrices
    static float** SimpleInverse(float** original);

    // Multiply a Vector3 by a 4 X 4 Matrix, returns the new altered Vector3
    static Vector3D MultiplyVectorByMatrix4(const Vector3D& v, float** matrix4x4, bool del);

private:
    static float** MakeNewArr();
};

void DeleteMatrix(float** matrix, int matHeight);
#pragma once
#include "Vector3D.h"
#define arrLen3x3 3
#define arrLen4x4 4

using namespace std;

struct Mat3x3
{
private:
    float m[3][3] = { 0 };
public:

    static float** Identity()
    {
        float** m = MakeNewArr();
        m[0][0] = 1;
        m[1][1] = 1;
        m[2][2] = 1;
        return m;
    }

    static float** Empty()
    {
        return MakeNewArr();
    }

    // Rotation X Matrix
    static float** RotationX(float theta)
    {
        float** m = MakeNewArr();
        m[0][0] = 1;
        m[1][1] = cos(theta);
        m[1][2] = -sin(theta);
        m[2][1] = sin(theta);
        m[2][2] = cos(theta);
        return m;
    }
    // Rotation Y Matrix
    static float** RotationY(float theta)
    {
        float** m = MakeNewArr();
        m[0][0] = cos(theta);
        m[0][2] = sin(theta);
        m[1][1] = 1;
        m[2][0] = -sin(theta);
        m[2][2] = cos(theta);
        return m;
    }
    // Rotation Z Matrix
    static float** RotationZ(float theta)
    {
        float** m = MakeNewArr();
        m[0][0] = cos(theta);
        m[0][1] = -sin(theta);
        m[1][0] = sin(theta);
        m[1][1] = cos(theta);
        m[2][2] = 1;
        return m;
    }
    // Rotation about any vector
    static float** Rotation(Vector3D axis, float theta)
    {
        float** m = MakeNewArr();
        Vector3D r = axis.Normalized();
        float c = cos(theta);
        float t = 1 - c;
        float s = sin(theta);
        m[0][0] = t * r.getX() * r.getX() + c;
        m[0][1] = t * r.getY() * r.getX() - s * r.getZ();
        m[0][2] = t * r.getZ() * r.getX() + s * r.getY();
        m[1][0] = t * r.getX() * r.getY() + s * r.getZ();
        m[1][1] = t * r.getY() * r.getY() + c;
        m[1][2] = t * r.getZ() * r.getY() - s * r.getX();
        m[2][0] = t * r.getX() * r.getZ() - s * r.getY();
        m[2][1] = t * r.getY() * r.getZ() + s * r.getX();
        m[2][2] = t * r.getZ() * r.getZ() + c;
        return m;
    }

    // Multiply a Vector3 by a 3 X 3 Matrix, returns the new altered Vector3
    static Vector3D MultiplyVectorByMatrix3(const Vector3D& v, float** matrix3x3, bool del) {
        Vector3D newV(0, 0, 0);
        newV.setX(v.x * matrix3x3[0][0] + v.y * matrix3x3[1][0] + v.z * matrix3x3[2][0]);
        newV.setY(v.x * matrix3x3[0][1] + v.y * matrix3x3[1][1] + v.z * matrix3x3[2][1]);
        newV.setZ(v.x * matrix3x3[0][2] + v.y * matrix3x3[1][2] + v.z * matrix3x3[2][2]);

        // delete the matrix is specified to not be used anymore
        if (del)
        {
            for (int i = 0; i < arrLen3x3; i++)
            {
                delete[] matrix3x3[i];
            }
            delete[] matrix3x3;
        }

        return newV;
    }

private:
    static float** MakeNewArr()
    {
        float** m = new float* [arrLen3x3];
        for (int i = 0; i < arrLen3x3; i++)
        {
            m[i] = new float[arrLen3x3];
        }
        m[0][0] = 0; m[0][1] = 0; m[0][2] = 0;
        m[1][0] = 0; m[1][1] = 0; m[1][2] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 0;
        return m;
    }
};


struct Mat4x4
{
    
public:
    float m[4][4] = { 0 };

    // Projection Matrix
    static float** Projection(float fNear, float fFar, float fov, float aspectRatio)
    {
        float** m = MakeNewArr();
        float fovRad = 1.0f / tan(fov * 0.5f / 180.0f * PI); // FOV converted to Radians for matrix multiplication
        
        m[0][0] = aspectRatio * fovRad;
        m[1][1] = fovRad;
        m[2][2] = fFar / (fFar - fNear);
        m[3][2] = (-fFar * fNear) / (fFar - fNear);
        m[2][3] = 1.0f;
        m[3][3] = 0.0f;
        return m;
    }

    // Multiply a Vector3 by a 4 X 4 Matrix, returns the new altered Vector3
    static Vector3D MultiplyVectorByMatrix4(const Vector3D& v, float** matrix4x4, bool del) {
        Vector3D alteredVector(0, 0, 0);
        alteredVector.setX(v.x * matrix4x4[0][0] + v.y * matrix4x4[1][0] + v.z * matrix4x4[2][0] + matrix4x4[3][0]);
        alteredVector.setY(v.x * matrix4x4[0][1] + v.y * matrix4x4[1][1] + v.z * matrix4x4[2][1] + matrix4x4[3][1]);
        alteredVector.setZ(v.x * matrix4x4[0][2] + v.y * matrix4x4[1][2] + v.z * matrix4x4[2][2] + matrix4x4[3][2]);
        float w = v.x * matrix4x4[0][3] + v.y * matrix4x4[1][3] + v.z * matrix4x4[2][3] + matrix4x4[3][3];

        // As objects move away from the user, their x and y values appear to change less. Dividing each component by w makes this happen.
        if (w != 0.0) {
            alteredVector.setX(alteredVector.getX() / w);
            alteredVector.setY(alteredVector.getY() / w);
            alteredVector.setZ(alteredVector.getZ() / w);
        }

        // delete the matrix is specified to not be used anymore
        if (del)
        {
            for (int i = 0; i < arrLen4x4; i++)
            {
                delete[] matrix4x4[i];
            }
            delete[] matrix4x4;
        }

        return alteredVector;
    }

private:
    static float** MakeNewArr()
    {
        float** m = new float* [arrLen4x4];
        for (int i = 0; i < arrLen4x4; i++)
        {
            m[i] = new float[arrLen4x4];
        }
        m[0][0] = 0; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
        m[1][0] = 0; m[1][1] = 0; m[1][2] = 0; m[1][3] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 0; m[2][3] = 0;
        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 0;
        return m;
    }
};
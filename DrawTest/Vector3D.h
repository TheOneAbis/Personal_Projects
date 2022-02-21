#pragma once
#include <iostream>
#define SFML_STATIC
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <windows.h>

#define PI 3.1415926f

using namespace sf;
using namespace std;


struct Vector3D
{
private:
    

public:
    float x, y, z;
    
    // Creates a new vector with an x, y, and z component
    Vector3D(float x, float y, float z);
    Vector3D();

    // Quick internal function that updates the vector's length whenever it's required
    float Length();
    float SqrLength();

    // Set the given vector's length to 1
    static void Normalize(Vector3D& v);

    // Return a new version of this vector with a length of 1
    Vector3D Normalized();

    // Add v1 + v2
    Vector3D operator+(Vector3D& v);

    Vector3D operator+(float value);

    // Add v2 to this v
    void operator+=(Vector3D& v);

    void operator+=(float value);

    // Subtract v1 - v2
    Vector3D operator-(const Vector3D& v);

    // Subtract v2 from this v
    void operator-=(Vector3D& v);
    
    Vector3D operator*(float amt);

    void operator*=(float amt);

    Vector3D operator/(float amt);

    void operator/=(float amt);

    // Calculate Cross Product of 2 3D vectors
    static Vector3D Cross(const Vector3D& v1, const Vector3D& v2);

    // Calculate the Dot Product of 2 vectors
    float operator*(Vector3D& v);

    // Calculate the Dot Product of 2 vectors
    float Dot(const Vector3D& v);

    static Vector3D IntersectPlane(Vector3D& plane_p, Vector3D& plane_n, Vector3D lineStart, Vector3D lineEnd);
    
    const float getX();
    const float getY();
    const float getZ();

    void setX(float newX);
    void setY(float newY);
    void setZ(float newZ);
};
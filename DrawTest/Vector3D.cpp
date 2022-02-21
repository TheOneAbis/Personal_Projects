#pragma once
#include "Vector3D.h"


    // Creates a new vector with an x, y, and z component
Vector3D::Vector3D(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3D::Vector3D()
{
    x = 0;
    y = 0;
    z = 0;
}

// Quick internal function that updates the vector's length whenever it's required
float Vector3D::Length()
{
    return std::sqrtf(x * x + y * y + z * z);
}

float Vector3D::SqrLength()
{
    return x * x + y * y + z * z;
}

// Set the given vector's length to 1
void Vector3D::Normalize(Vector3D& v)
{
    float len = v.Length();
    if (len != 0)
    {
        v.x /= len;
        v.y /= len;
        v.z /= len;
    }
}

// Return a new version of this vector with a length of 1
Vector3D Vector3D::Normalized()
{
    float len = Length();
    return (len == 0) ? Vector3D(0, 0, 0) : Vector3D(x / len, y / len, z / len);
}

// Add v1 + v2
Vector3D Vector3D::operator+(Vector3D& v)
{
    return Vector3D(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3D Vector3D::operator+(float value)
{
    return Vector3D(this->x + value, this->y + value, this->z + value);
}

// Add v2 to this v
void Vector3D::operator+=(Vector3D& v)
{
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
}

void Vector3D::operator+=(float value)
{
    this->x += value;
    this->y += value;
    this->z += value;
}

// Subtract v1 - v2
Vector3D Vector3D::operator-(const Vector3D& v)
{
    return Vector3D(this->x - v.x, this->y - v.y, this->z - v.z);
}

// Subtract v2 from this v
void Vector3D::operator-=(Vector3D& v)
{
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
}

Vector3D Vector3D::operator*(float amt)
{
    return Vector3D(this->x * amt, this->y * amt, this->z * amt);
}

void Vector3D::operator*=(float amt)
{
    x *= amt;
    y *= amt;
    z *= amt;
}

Vector3D Vector3D::operator/(float amt)
{
    return Vector3D(this->x / amt, this->y / amt, this->z / amt);
}

void Vector3D::operator/=(float amt)
{
    x /= amt;
    y /= amt;
    z /= amt;
}

// Calculate Cross Product of 2 3D vectors
Vector3D Vector3D::Cross(const Vector3D& v1, const Vector3D& v2)
{
    return Vector3D((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
}

// Calculate the Dot Product of 2 vectors
float Vector3D::operator*(Vector3D& v) {
    return this->x * v.x + this->y * v.y + this->z * v.z;
}

// Calculate the Dot Product of 2 vectors
float Vector3D::Dot(const Vector3D& v) {
    return this->x * v.x + this->y * v.y + this->z * v.z;
}

 Vector3D Vector3D::IntersectPlane(Vector3D& plane_p, Vector3D& plane_n, Vector3D lineStart, Vector3D lineEnd)
{
    float plane_d = -(plane_n.Normalized() * plane_p);
    float ad = lineStart * plane_n;
    float bd = lineEnd * plane_n;
    float t = (-plane_d - ad) / (bd - ad);
    Vector3D lineStartToEnd = lineEnd - lineStart;
    Vector3D lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

const float Vector3D::getX() { return x; }
const float Vector3D::getY() { return y; }
const float Vector3D::getZ() { return z; }

void Vector3D::setX(float newX) { x = newX; }
void Vector3D::setY(float newY) { y = newY; }
void Vector3D::setZ(float newZ) { z = newZ; }
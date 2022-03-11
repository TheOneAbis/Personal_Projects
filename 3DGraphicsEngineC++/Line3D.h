#pragma once
#include "Vector3D.h"


struct Line3D
{
private:
    Vector3D points[2];
    int count;
    VertexArray projected;
    float X, Y, Z;

public:
    Vector3D viewed[2];

    // Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
    Line3D(Vector3D pStart, Vector3D pEnd);

    Vector3D operator[](int i);

    VertexArray GetProjected();

    void SetProjected(VertexArray& newLine);

    int Count();
};
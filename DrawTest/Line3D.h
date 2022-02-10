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
    Line3D(Vector3D pStart, Vector3D pEnd)
    {
        points[0] = pStart;
        points[1] = pEnd;
        projected = VertexArray(Lines, 2);
        projected[0] = (Vector2f());
        projected[1] = (Vector2f());
        count = 2;
    }

    Vector3D operator[](int i) { return points[i]; }

    VertexArray GetProjected() { return projected; }
    void SetProjected(VertexArray& newLine) { this->projected = newLine; }

    int Count() { return count; }
};
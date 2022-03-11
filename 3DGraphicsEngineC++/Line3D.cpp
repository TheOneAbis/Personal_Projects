#pragma once
#include "Line3D.h"


// Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
Line3D::Line3D(Vector3D pStart, Vector3D pEnd)
{
    points[0] = pStart;
    points[1] = pEnd;
    projected = VertexArray(Lines, 2);
    projected[0] = (Vector2f());
    projected[1] = (Vector2f());
    count = 2;
}

Vector3D Line3D::operator[](int i) { return points[i]; }

VertexArray Line3D::GetProjected() { return projected; }

void Line3D::SetProjected(VertexArray& newLine) { this->projected = newLine; }

int Line3D::Count() { return count; }
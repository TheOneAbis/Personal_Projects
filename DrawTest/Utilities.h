#pragma once
#include "Triangle3D.h"
#include "Line3D.h"

float Clamp(float num, float min, float max);

Vector3D GlobalRight();
Vector3D GlobalUp();
Vector3D GlobalForward();

// returns how many triangles to create from original triangle when clipped against a plane
int ClipTriAgainstPlane(Vector3D planeP, Vector3D planeN, SimpleTri3D& originalTri, SimpleTri3D& newTri1, SimpleTri3D& newTri2);

// returns how many triangles to create from original triangle when clipped against a plane
Line3D ClipLineAgainstPlane(Vector3D planeP, Vector3D planeN, Line3D originalLine);
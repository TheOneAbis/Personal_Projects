#pragma once
#include "Triangle3D.h"
#include "Line3D.h"

float Clamp(float num, float min, float max)
{
	if (num < min) return min;
	if (num > max) return max;
	return num;
}

static Vector3D GlobalRight() { return Vector3D(1, 0, 0); }
static Vector3D GlobalUp() { return Vector3D(0, 1, 0); }
static Vector3D GlobalForward() { return Vector3D(0, 0, 1); }


// returns how many triangles to create from original triangle when clipped against a plane
static int ClipTriAgainstPlane(Vector3D planeP, Vector3D planeN, Triangle3D& originalTri, Triangle3D& newTri1, Triangle3D& newTri2)
{
	planeN = planeN.Normalized();

	auto distToPlane = [&](Vector3D p)
	{ return (planeN.x * p.x + planeN.y * p.y + planeN.z * p.z - (planeN * planeP)); };
	Vector3D behindPoints[3]; int numPointsBehind = 0;
	Vector3D inFrontPoints[3]; int numPointsInFront = 0;

	// Check signed distance of po0nts from plane - >= 0 means in front of/on plane
	for (int i = 0; i < originalTri.Count(); i++)
	{
		float pointDist = distToPlane(originalTri[i]);
		if (pointDist >= 0)
			inFrontPoints[numPointsInFront++] = originalTri[i];
		else
			behindPoints[numPointsBehind++] = originalTri[i];
	}
	
	// If no points exist in front, clip whole triangle
	if (numPointsInFront == 0)
		return 0; // no triangles
	// If all points exist in front, do nothing
	if (numPointsInFront == 3)
	{
		newTri1 = originalTri; // original triangle
		return 1; 
	}
	// Clip outside, return the tri's tip that's still inside
	if (numPointsInFront == 1 && numPointsBehind == 2)
	{
		newTri1[0] = inFrontPoints[0]; // keep inside point
		// new points are where tri's lines intersect w/ plane
		newTri1[1] = Vector3D::IntersectPlane(planeP, planeN, inFrontPoints[0], behindPoints[0]);
		newTri1[2] = Vector3D::IntersectPlane(planeP, planeN, inFrontPoints[0], behindPoints[1]);
		return 1;
	}
	// CLip outside, return the 2 tris that make up the original tri's base that's still inside
	if (numPointsInFront == 2 && numPointsBehind == 1)
	{
		newTri1[0] = inFrontPoints[0];
		newTri1[1] = inFrontPoints[1];
		newTri1[2] = Vector3D::IntersectPlane(planeP, planeN, inFrontPoints[0], behindPoints[0]);

		newTri2[0] = inFrontPoints[1];
		newTri2[1] = newTri1[2];
		newTri2[2] = Vector3D::IntersectPlane(planeP, planeN, inFrontPoints[1], behindPoints[0]);
		return 2;
	}
}

// returns how many triangles to create from original triangle when clipped against a plane
static Line3D ClipLineAgainstPlane(Vector3D planeP, Vector3D planeN, Line3D originalLine)
{
	planeN = planeN.Normalized();
	Vector3D p[2];

	auto distToPlane = [&](Vector3D p)
	{ return (planeN.x * p.x + planeN.y * p.y + planeN.z * p.z - (planeN * planeP)); };

	// Check signed distance of points from plane - >= 0 means in front of/on plane
	for (int i = 0; i < originalLine.Count(); i++)
	{
		if (distToPlane(originalLine[i]) >= 0)
			p[i] = originalLine[i];
		else
			p[i] = Vector3D::IntersectPlane(planeP, planeN, originalLine[1], originalLine[0]);
	}
	
	return Line3D(p[0], p[1]);
}
#pragma once
#include "Mesh.h"

class Torus : public Mesh
{
public:

	Torus(float radius, float ringRadius, int radialDivisions, int ringDivisions, const Vector3D& offset);
};
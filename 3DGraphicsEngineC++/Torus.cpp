#include "Torus.h"

Torus::Torus(float radius, float ringRadius, int radialDivisions, int ringDivisions, const Vector3D& offset)
	: Mesh(offset)
{
	Vector3D newVMid;

	// Create vertices for torus
	for (float theta = 0.0f; theta <= 2.0f * PI; theta += (2.0f*PI) / (float)radialDivisions) // loop through each edge ring
	{
		newVMid = Vector3D(cos(theta), 0, sin(theta)) * radius;

		for (float phi = 0.0f; phi <= 2.0f * PI; phi += (2.0f * PI) / (float)ringDivisions) // each vertex per ring
		{
			Vector3D newV = Vector3D(cos(phi) * cos(theta), sin(phi), cos(phi) * sin(theta)) * ringRadius;
			vertices.push_back(newVMid + newV);
		}
	}

	// Set torus triangles w/ vertices
	for (int i = ringDivisions + 2; i < vertices.size(); i++)
	{
		tris.push_back(Triangle3D(&vertices[i - ringDivisions - 2], &vertices[(i - ringDivisions - 1)], &vertices[i - 1]));
		tris.push_back(Triangle3D(&vertices[(i - ringDivisions - 1)], &vertices[i] , &vertices[i - 1]));
	}

	// Offset it out to its XYZ position
	for (Vector3D& vec : vertices)
	{
		Vector3D temp = (this->Position + this->Pivot);
		vec += temp;
	}
}
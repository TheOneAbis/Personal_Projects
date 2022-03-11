#include "Triangle3D.h"


VertexArray SimpleTri3D::GetProjected() { return projected; }

// Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
Triangle3D::Triangle3D(Vector3D* p1, Vector3D* p2, Vector3D* p3)
{
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    centerZ = (p1->getZ() + p2->getZ() + p3->getZ()) / 3;
    normal = Vector3D::Cross(*points[1] - *points[0], *points[2] - *points[0]).Normalized();
}

Vector3D Triangle3D::operator[](int i) { return *points[i]; }

Vector3D Triangle3D::Normal() { return normal; }

void Triangle3D::SetNormal(Vector3D value) { this->normal = value; }

Vector3D Triangle3D::CamNormal() { return camNormal; }

void Triangle3D::SetCamNormal(Vector3D value) { this->camNormal = value; }

int Triangle3D::Count() { return points.size(); }

float Triangle3D::getCenterZ() { return centerZ; }

void Triangle3D::setCenterZ(float value) { centerZ = value; }
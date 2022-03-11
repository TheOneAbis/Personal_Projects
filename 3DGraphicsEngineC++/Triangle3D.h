#pragma once
#include "Vector3D.h"


struct SimpleTri3D
{
    Vector3D p[3];
    VertexArray projected;

    VertexArray GetProjected();
};

struct Triangle3D
{
private:
    float centerZ;
    Vector3D normal, camNormal;

public:

    std::vector<Vector3D*> points;
    std::vector<Vector3D> tempPoints;
    SimpleTri3D viewed;

    // Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
    Triangle3D(Vector3D* p1, Vector3D* p2, Vector3D* p3);

    Vector3D operator[](int i);

    Vector3D Normal();

    void SetNormal(Vector3D value);

    Vector3D CamNormal();

    void SetCamNormal(Vector3D value);

    int Count();

    float getCenterZ();

    void setCenterZ(float value);
};
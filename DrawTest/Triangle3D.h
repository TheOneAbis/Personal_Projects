#pragma once
#include "Vector3D.h"

struct Triangle3D
{
private:
    std::vector<Vector3D*> points;
    std::vector<Vector3D> tempPoints;

    VertexArray projected;
    float centerZ;
    Vector3D normal, camNormal;
    float X, Y, Z;
    bool usingPtrs;

public:

    Vector3D viewed[3];

    // Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
    Triangle3D(Vector3D* p1, Vector3D* p2, Vector3D* p3)
    {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        projected = VertexArray(Triangles, 3);
        projected[0] = (Vector2f());
        projected[1] = (Vector2f());
        projected[2] = (Vector2f());
        centerZ = (p1->getZ() + p2->getZ() + p3->getZ()) / 3;
        normal = Vector3D::Cross(*points[1] - *points[0], *points[2] - *points[0]).Normalized();
        X = p1->getX();
        Y = p1->getY();
        Z = p1->getZ();
        usingPtrs = true;
    }

    Triangle3D()
    {
        tempPoints.push_back(Vector3D(1, 0, 0));
        tempPoints.push_back(Vector3D(0, 1, 0));
        tempPoints.push_back(Vector3D(0, 0, 1));
        projected = VertexArray(Triangles, 3);
        projected[0] = (Vector2f());
        projected[1] = (Vector2f());
        projected[2] = (Vector2f());
        usingPtrs = false;
    }

    Vector3D operator[](int i) 
    { 
        return usingPtrs ? *points[i] : tempPoints[i];
    }

    Vector3D Normal() { return normal; }
    void SetNormal(Vector3D value) { this->normal = value; }

    Vector3D CamNormal() { return camNormal; }
    void SetCamNormal(Vector3D value) { this->camNormal = value; }

    VertexArray GetProjected() { return projected; }
    void SetProjected(VertexArray& newTri) { this->projected = newTri; }

    int Count() 
    { 
        return usingPtrs ? points.size() : tempPoints.size();
    }

    float getCenterZ() { return centerZ; }
    void setCenterZ(float value) { centerZ = value; }
    float getX() { return X; }
    float getY() { return Y; }
    float getZ() { return Z; }
};
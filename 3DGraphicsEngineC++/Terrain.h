#pragma once
#include "Triangle3D.h"
#include "Matrices.h"
#include "Utilities.h"

class Terrain
{
private:

    vector<Vector3D> vertices;
    vector<Triangle3D> tris;
    Vector3D Pivot, Position;

public:

    Terrain(Vector3D offset, int vertRows, int vertCols, int vertSpacing);

    void FillBuffer(vector<Triangle3D*>& buffer);

    vector<Vector3D>& GetVertices();

};


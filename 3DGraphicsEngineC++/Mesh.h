#pragma once
#include "Triangle3D.h"
#include "Matrices.h"
#include "Utilities.h"

#include <fstream>
#include <strstream>


class Mesh
{
protected:

    vector<Vector3D> vertices;
    vector<Triangle3D> tris;
    Vector3D Pivot, Position;

private:
    
    Vector3D Right, Up, Forward;

    // Loads the OBJ file into the mesh object
    bool LoadOBJ(string fileName);

public:

    // Creates a new Mesh Object from a Loaded OBJ file
    Mesh(string objFile, const Vector3D& offset);

    Mesh(const Vector3D& offset);

    Vector3D GetPosition();

    void Move(float x, float y, float z);

    // Rotate the mesh by z, x, then y, using either local or global coords
    void Rotate(float xRad, float yRad, float zRad, bool isLocal);

    // Add this mesh's triangles to the tri buffer
    void FillBuffer(vector<Triangle3D*>& buffer);

    vector<Triangle3D> GetTris();
};
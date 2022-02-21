#pragma once
#include "Triangle3D.h"
#include "Matrices.h"
#include "Utilities.h"
#include "Camera.h"

#include <fstream>
#include <strstream>


class Mesh
{
private:
    std::vector<Triangle3D> tris;
    std::vector<Vector3D> vertices;
    Vector3D Pivot, Position, Right, Up, Forward;
    Camera* cam;

    std::vector<SimpleTri3D> trisToDraw;

public:

    Vector3D GetPosition();

    void Move(float x, float y, float z);

    // Creates a new Mesh Object from a Loaded OBJ file
    Mesh(string objFile, const Vector3D& offset, Camera* cam);
    
    // Loads the OBJ file into the mesh object
    bool LoadOBJ(string fileName);

    // Rotate the mesh by z, x, then y, using either local or global coords
    void Rotate(float xRad, float yRad, float zRad, bool isLocal);

    void UpdateViewSpace();

    // Project the 3D mesh onto the screen 
    // (Computes the viewed version of it for the camera, then projects)
    void UpdateDisplay(float fNear, float fFar, float fov, Vector3D& lightDir, RenderWindow* window);

    void Draw(RenderWindow* window);

    vector<Triangle3D> GetTris();
};
#pragma once
#include "Triangle3D.h"
#include "Matrices.h"
#include "Utilities.h"
#include "Camera.h"

#include <fstream>
#include <strstream>
#include <queue>


class Mesh
{
protected:
    std::vector<Vector3D> vertices;
    std::vector<Triangle3D> tris;
    Vector3D Pivot, Position;

private:
    
    Vector3D Right, Up, Forward;
    Camera* cam;

    std::vector<SimpleTri3D> trisToDraw;

    // Loads the OBJ file into the mesh object
    bool LoadOBJ(string fileName);

public:

    // Creates a new Mesh Object from a Loaded OBJ file
    Mesh(string objFile, const Vector3D& offset, Camera* cam);

    Mesh(const Vector3D& offset, Camera* cam);

    Vector3D GetPosition();

    void Move(float x, float y, float z);

    // Rotate the mesh by z, x, then y, using either local or global coords
    void Rotate(float xRad, float yRad, float zRad, bool isLocal);

    void UpdateViewSpace();

    // Add this mesh's triangles to the tri buffer
    void FillBuffer(vector<Triangle3D*>& buffer);

    // Project the 3D mesh onto the screen 
    // (Computes the viewed version of it for the camera, then projects)
    void UpdateDisplay(float fNear, float fFar, float fov, Vector3D& lightDir, RenderWindow* window, vector<SimpleTri3D>& triBuffer);

    void Draw(RenderWindow* window);

    vector<Triangle3D> GetTris();
};
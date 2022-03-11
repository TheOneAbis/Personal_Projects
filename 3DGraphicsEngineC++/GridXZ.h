#pragma once
#include "Matrices.h"
#include "Utilities.h"
#include "Camera.h"
#include "Line3D.h"


class GridXZ
{
private:
    std::vector<Line3D> lines;
    std::vector<Line3D> clippedLines, newClippedLines;
    const int gridSize = 10;
    const int numLines = 50;
public:
    // Creates a new Mesh object from a Loaded OBJ file
    GridXZ();

    // Project the grid onto the screen
    void UpdateDisplay(float fNear, float fFar, float fov, Camera& camera, RenderWindow* window);

    void Draw(RenderWindow* window);

    vector<Line3D> GetLines();
};
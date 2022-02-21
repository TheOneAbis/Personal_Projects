#pragma once
#include "GridXZ.h"


// Creates a new Mesh object from a Loaded OBJ file
GridXZ::GridXZ()
{
    // Z-axis lines
    for (int i = -50; i <= 50; i++)
        lines.push_back(Line3D(Vector3D((float)(i * gridSize), 0, (float)(-numLines * gridSize)), 
            Vector3D((float)(i * gridSize), 0, (float)(numLines * gridSize))));
    // X-axis lines
    for (int i = -50; i <= 50; i++)
        lines.push_back(Line3D(Vector3D((float)(-numLines * gridSize), 0, (float)(i * gridSize)), 
            Vector3D((float)(numLines * gridSize), 0, (float)(i * gridSize))));
}

// Project the grid onto the screen
void GridXZ::UpdateDisplay(float fNear, float fFar, float fov, Camera& camera, RenderWindow* window)
{
    float sceneWidth = (float)window->getSize().x, sceneHeight = (float)window->getSize().y;
    float aspectRatio = sceneHeight / sceneWidth;

    newClippedLines.clear();

    for (Line3D& line : lines)
    {
        VertexArray displayLine(Lines, 2);
        for (int i = 0; i < line.Count(); i++)
        {
            line.viewed[i] = Mat3x3::MultiplyVectorByMatrix3(
                Mat3x3::MultiplyVectorByMatrix3(
                    line[i] - camera.position, Mat3x3::Rotation(camera.Right, camera.WorldPhi), true),
                Mat3x3::Rotation(GlobalUp(), camera.WorldTheta), true);
        }

        Line3D viewed(line.viewed[0], line.viewed[1]);
        Line3D clipped = ClipLineAgainstPlane(Vector3D(0, 0, fNear), Vector3D(0, 0, 1), viewed);

        for (int i = 0; i < line.Count(); i++)
        {
            // Project triangles to 2D screen using Projection
            Vector3D projVector = Mat4x4::MultiplyVectorByMatrix4(clipped[i], Mat4x4::Projection(fNear, fFar, fov, aspectRatio), true);

            projVector.x += 1.0; // Move mesh to middle of screen
            projVector.y += 1.0;
            projVector.x *= 0.5f * sceneWidth; // scale it out from 1px
            projVector.y *= 0.5f * sceneHeight;

            // Create vertex with vector position and shade color, append it to triangle VertexArrays
            Vertex projVector2D(Vector2f(projVector.x, projVector.y), Color::Black);
            displayLine[i] = (projVector2D);
        }

        clipped.SetProjected(displayLine);
        newClippedLines.push_back(clipped);
    }
    clippedLines = newClippedLines;
}

void GridXZ::Draw(RenderWindow* window)
{
    // Draw the lines
    for (auto line : clippedLines)
        window->draw(line.GetProjected());
}

vector<Line3D> GridXZ::GetLines() { return lines; }
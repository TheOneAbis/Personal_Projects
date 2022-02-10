#pragma once
#include "Matrices.h"
#include "Utilities.h"
#include "Camera.h"
#include "Line3D.h"


class GridXZ
{
private:
    std::vector<Line3D> lines;
    const int gridSize = 10;
    const int numLines = 50;
public:
    // Creates a new Mesh Object from a Loaded OBJ file
    GridXZ()
    {
        // Z-axis lines
        for (int i = -50; i <= 50; i ++)
            lines.push_back(Line3D(Vector3D(i * gridSize, 0, -numLines * gridSize), Vector3D(i * gridSize, 0, numLines * gridSize)));
        // X-axis lines
        for (int i = -50; i <= 50; i++)
            lines.push_back(Line3D(Vector3D(-numLines * gridSize, 0, i * gridSize), Vector3D(numLines * gridSize, 0, i * gridSize)));
    }

    // Project the grid onto the screen
    void Draw(float fNear, float fFar, float fov, Camera& camera, RenderWindow* window)
    {
        float sceneWidth = (float)window->getSize().x, sceneHeight = (float)window->getSize().y;
        float aspectRatio = sceneHeight / sceneWidth;

        vector<Line3D> clippedLines;

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
                projVector.x *= 0.5 * sceneWidth; // scale it out from 1px
                projVector.y *= 0.5 * sceneHeight;

                // Create vertex with vector position and shade color, append it to triangle VertexArrays
                Vertex projVector2D(Vector2f(projVector.x, projVector.y), Color::Black);
                displayLine[i] = (projVector2D);
            }

            clipped.SetProjected(displayLine);
            clippedLines.push_back(clipped);
        }
        // Draw the lines
        for (auto line : clippedLines)
            window->draw(line.GetProjected());
    }

    vector<Line3D> GetLines() { return lines; }
};
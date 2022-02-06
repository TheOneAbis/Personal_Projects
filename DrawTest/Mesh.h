#pragma once
#include "Triangle3D.h"
#include "Matrices.h"
#include <fstream>
#include <strstream>
#include "Utilities.h"

class Mesh
{
private:
    std::vector<Triangle3D> tris;
    std::vector<Vector3D> vertices;
    Vector3D pivot, offset, Right, Up, Forward;

public:
    static Vector3D GlobalRight() { return Vector3D(1, 0, 0); }
    static Vector3D GlobalUp() { return Vector3D(0, 1, 0); }
    static Vector3D GlobalForward() { return Vector3D(0, 0, 1); }

    Mesh(string objFile, const Vector3D& offset)
    {
        this->pivot = Vector3D(0, 0, 0);
        this->offset = offset;

        Right = Vector3D(1, 0, 0);
        Up = Vector3D(0, 1, 0);
        Forward = Vector3D(0, 0, 1);

        LoadOBJ(objFile);

        // Offset it out so it's not on top of the camera
        for (Vector3D& vec : vertices)
        {
            Vector3D temp = (this->offset + pivot);
            vec += temp;
        }
    }

    bool LoadOBJ(string fileName)
    {
        ifstream file(fileName);

        if (!file.is_open())
            return false;

        while (!file.eof())
        {
            char line[128];
            file.getline(line, 128);
            strstream s;
            s << line; // grab new line from file

            char junk; // plcce to throw out the v or f label

            // Add new vertex
            if (line[0] == 'v')
            {
                float x, y, z;
                s >> junk >> x >> y >> z; // Extract line data
                vertices.push_back(Vector3D(x, y, z));
            }
            // Add new face
            if (line[0] == 'f')
            {
                string f[3];
                int fInt[3];
                s >> junk >> f[0] >> f[1] >> f[2]; // Extract line data
                for (int i = 0; i < sizeof(f) / sizeof(f[0]); i++) // Omit the vt, not used
                {
                    f[i] = f[i].substr(0, f[i].find('/'));
                    fInt[i] = stoi(f[i]);
                }
                tris.push_back(Triangle3D(&vertices[fInt[0] - 1], &vertices[fInt[1] - 1], &vertices[fInt[2] - 1]));
            }
        }
    }

    void Rotate(float xRad, float yRad, float zRad, bool isLocal)
    {
        if (!isLocal) 
        {
            Up = Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
                Up, Mat3x3::Rotation(Mesh::GlobalForward(), zRad), true), Mat3x3::Rotation(Mesh::GlobalRight(), xRad), true), Mat3x3::Rotation(Mesh::GlobalUp(), yRad), true);
            Forward = Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
                Forward, Mat3x3::Rotation(Mesh::GlobalForward(), zRad), true), Mat3x3::Rotation(Mesh::GlobalRight(), xRad), true), Mat3x3::Rotation(Mesh::GlobalUp(), yRad), true);
            Right = Vector3D::Cross(Forward, Up);
        }

        for (Vector3D& vec : vertices)
        {
            /*            LEARNED AN IMPORTANT LESSON HERE
            NEVER set vec(or anything used in a different thread for that matter) 
            to something that isn't its final value in multithreaded applications 
            */
            Vector3D temp = vec; 

            // Bring vertex to origin point before rotating
            temp -= offset;

            // Rotate mesh around Z axis, then X axis, then Y axis around local or global axes
            temp = isLocal ? Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
                temp, Mat3x3::Rotation(Forward, zRad), true), Mat3x3::Rotation(Right, xRad), true), Mat3x3::Rotation(Up, yRad), true) :
                Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
                    temp, Mat3x3::Rotation(Mesh::GlobalForward(), zRad), true), Mat3x3::Rotation(Mesh::GlobalRight(), xRad), true), Mat3x3::Rotation(Mesh::GlobalUp(), yRad), true);

            // Return vertex to offset point
            temp += offset;
            vec = temp;
        }
        for (Triangle3D &tri : tris)
            tri.SetNormal(Vector3D::Cross(tri[1] - tri[0], tri[2] - tri[0]).Normalized());
    }

    // Project the 3D mesh onto the screen
    void Draw(float fNear, float fFar, float fov, Vector3D& camera, Vector3D& lightDir, RenderWindow* window) 
    {
        float sceneWidth = (float)window->getSize().x, sceneHeight = (float)window->getSize().y;
        float aspectRatio = sceneHeight / sceneWidth;

        vector<Triangle3D*> trisToDraw;
        for (Triangle3D& tri : tris) 
        {
            // Only project & draw visible triangles (if angle between normal and any pt on the tri relative to camera is <= 90deg)
            if (tri.Normal().Dot(tri[0] - camera) < 0.0) 
            {
                float shade = Clamp(255 * (tri.Normal() * lightDir), 30, 225);
                trisToDraw.push_back(&tri);

                VertexArray displayTri(Triangles, 3);
                for (int i = 0; i < tri.Count(); i++) 
                {
                    // Project triangles to 2D screen using Projection
                    Vector3D projVector = Mat4x4::MultiplyVectorByMatrix4(tri[i],  Mat4x4::Projection(fNear, fFar, fov, aspectRatio), true);
                    projVector.x += 1.0; // Move mesh to middle of screen
                    projVector.y += 1.0;
                    projVector.x *= 0.5 * sceneWidth; // scale it out from 1px
                    projVector.y *= 0.5 * sceneHeight;

                    // Create vertex with vector position and shade color, append it to triangle VertexArrays
                    Vertex projVector2D(Vector2f(projVector.x, projVector.y), Color(shade, shade, shade, 255));
                    displayTri[i] = (projVector2D);
                }
                tri.SetProjected(displayTri);
                tri.setCenterZ((tri[0].getZ() + tri[1].getZ() + tri[2].getZ()) / 3.0f);
            }
        }
        // Sort draw order, draw tris farthest to nearest
        sort(trisToDraw.begin(), trisToDraw.end(), [](Triangle3D* t1, Triangle3D* t2)
        {
                return t1->getCenterZ() > t2->getCenterZ();
        });

        // Draw the triangles
        for (auto tri : trisToDraw)
            window->draw(tri->GetProjected());
    }

    vector<Triangle3D> GetTris() { return tris; }

    void SetPivot(Vector3D& newLoc) { this->pivot = newLoc; }
};
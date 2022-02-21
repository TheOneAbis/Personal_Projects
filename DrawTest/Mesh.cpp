#pragma once
#include "Mesh.h"


Vector3D Mesh::GetPosition() { return Position; }

void Mesh::Move(float x, float y, float z)
{
    Vector3D offset(x, y, z);
    Position += offset;
    for (Vector3D& vec : vertices)
        vec += offset;
}

// Creates a new Mesh Object from a Loaded OBJ file
Mesh::Mesh(string objFile, const Vector3D& offset, Camera* cam)
{
    this->cam = cam;
    this->Pivot = Vector3D(0, 0, 0);
    this->Position = offset;

    Right = Vector3D(1, 0, 0);
    Up = Vector3D(0, 1, 0);
    Forward = Vector3D(0, 0, 1);

    LoadOBJ(objFile);

    // Offset it out so it's not on top of the camera
    for (Vector3D& vec : vertices)
    {
        Vector3D temp = (this->Position + this->Pivot);
        vec += temp;
    }
}

// Loads the OBJ file into the mesh object
bool Mesh::LoadOBJ(string fileName)
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

// Rotate the mesh by z, x, then y, using either local or global coords
void Mesh::Rotate(float xRad, float yRad, float zRad, bool isLocal)
{
    if (!isLocal)
    {
        Up = Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
            Up, Mat3x3::Rotation(GlobalForward(), zRad), true), Mat3x3::Rotation(GlobalRight(), xRad), true), Mat3x3::Rotation(GlobalUp(), yRad), true);
        Forward = Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
            Forward, Mat3x3::Rotation(GlobalForward(), zRad), true), Mat3x3::Rotation(GlobalRight(), xRad), true), Mat3x3::Rotation(GlobalUp(), yRad), true);
        Right = Vector3D::Cross(Forward, Up);
    }

    for (Vector3D& vec : vertices)
    {
        /*                              NOTE
        NEVER set vec(or anything used in a different thread for that matter)
        to something that isn't its final value in multithreaded applications */
        Vector3D temp = vec;

        // Bring vertex to origin point before rotating
        temp -= Position;

        // Rotate mesh around Z axis, then X axis, then Y axis around local or global axes
        temp = isLocal ? Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
            temp, Mat3x3::Rotation(Forward, zRad), true), Mat3x3::Rotation(Right, xRad), true), Mat3x3::Rotation(Up, yRad), true) :
            Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(Mat3x3::MultiplyVectorByMatrix3(
                temp, Mat3x3::Rotation(GlobalForward(), zRad), true), Mat3x3::Rotation(GlobalRight(), xRad), true), Mat3x3::Rotation(GlobalUp(), yRad), true);

        // Return vertex to offset point
        temp += Position;
        vec = temp;
    }
    //UpdateViewSpace();
}

void Mesh::UpdateViewSpace()
{
    //float** cameraMat = Mat4x4::PointAt(camera.position, camera.Target, camera.GetUp());
    //float** viewMat = Mat4x4::SimpleInverse(cameraMat);
    for (Triangle3D& tri : tris)
    {
        tri.SetNormal(Vector3D::Cross(tri[1] - tri[0], tri[2] - tri[0]).Normalized());

        // Calculate triangle's viewed triangle points
        for (short i = 0; i < tri.Count(); i++)
        {
            tri.viewed.p[i] = Mat3x3::MultiplyVectorByMatrix3(
                Mat3x3::MultiplyVectorByMatrix3(
                    tri[i] - cam->position, Mat3x3::Rotation(cam->Right, cam->WorldPhi), true),
                Mat3x3::Rotation(GlobalUp(), cam->WorldTheta), true);
        }
        tri.SetCamNormal(Vector3D::Cross(tri.viewed.p[1] - tri.viewed.p[0], tri.viewed.p[2] - tri.viewed.p[0]).Normalized());
    }
    //DeleteMatrix(cameraMat, 4); // Delete the camera PointAt matrix array
    //DeleteMatrix(viewMat, 4); // Delete the Camera LookAt matrix array
}

// Project the 3D mesh onto the screen 
// (Computes the viewed version of it for the camera, then projects)
void Mesh::UpdateDisplay(float fNear, float fFar, float fov, Vector3D& lightDir, RenderWindow* window)
{
    float sceneWidth = (float)window->getSize().x, sceneHeight = (float)window->getSize().y;
    float aspectRatio = sceneHeight / sceneWidth;

    std::vector<SimpleTri3D> newTrisToDraw;

    for (Triangle3D& tri : tris)
    {
        //if (tri.getX() == tris[0].getX()) cout << tri.pointsRelToCam[0].x << " " << tri.pointsRelToCam[0].y << " " << tri.pointsRelToCam[0].z << endl;

        // Only project & draw visible triangles (if angle between normal and any pt on the tri relative to camera is <= 90deg)
        if (tri.CamNormal().Dot(tri.viewed.p[0].Normalized()) < 0.0)
        {
            int shade = (int)Clamp(255 * (tri.Normal() * lightDir), 30, 225);

            SimpleTri3D clipped[2];
            int clippedTrisAmt = ClipTriAgainstPlane(Vector3D(0, 0, fNear), Vector3D(0, 0, 1), tri.viewed, clipped[0], clipped[1]);

            // Project the tri's clipped tris
            for (int n = 0; n < clippedTrisAmt; n++)
            {
                VertexArray displayTri(Triangles, 3);
                for (int i = 0; i < 3; i++)
                {
                    // Project triangles to 2D screen using Projection
                    Vector3D projVector = Mat4x4::MultiplyVectorByMatrix4(clipped[n].p[i], Mat4x4::Projection(fNear, fFar, fov, aspectRatio), true);

                    projVector.x += 1.0; // Move mesh to middle of screen
                    projVector.y += 1.0;
                    projVector.x *= 0.5f * sceneWidth; // scale it out from 1px
                    projVector.y *= 0.5f * sceneHeight;

                    // Create vertex with vector position and shade color, append it to triangle VertexArrays
                    Vertex projVector2D(Vector2f(projVector.x, projVector.y), Color(shade, shade, shade, 255));
                    displayTri[i] = (projVector2D);
                }
                clipped[n].projected = displayTri;
                newTrisToDraw.push_back(clipped[n]);
            }
        }
    }

    // Sort draw order, draw tris farthest to nearest
    sort(newTrisToDraw.begin(), newTrisToDraw.end(), [](SimpleTri3D& t1, SimpleTri3D& t2)
        {
            float t1Z = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float t2Z = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return t1Z > t2Z;
        });

    trisToDraw = newTrisToDraw;
}

void Mesh::Draw(RenderWindow* window)
{
    // Draw the triangles
    for (SimpleTri3D& tri : trisToDraw)
    {
        window->draw(tri.GetProjected());
    }
}

vector<Triangle3D> Mesh::GetTris() { return tris; }
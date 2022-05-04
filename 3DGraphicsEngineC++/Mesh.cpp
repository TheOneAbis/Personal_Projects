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
Mesh::Mesh(string objFile, const Vector3D& offset)
{
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

// Creates new mesh (to be used by child classes)
Mesh::Mesh(const Vector3D& offset)
{
    this->Pivot = Vector3D(0, 0, 0);
    this->Position = offset;

    Right = Vector3D(1, 0, 0);
    Up = Vector3D(0, 1, 0);
    Forward = Vector3D(0, 0, 1);
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

        char junk; // place to throw out the v or f label

        // Add new vertex
        if (line[0] == 'v')
        {
            float x, y, z;
            s >> junk >> x >> y >> z; // Extract line data
            vertices.push_back(Vector3D(x, -y, z));
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
            tris.push_back(Triangle3D(&vertices[fInt[2] - 1], &vertices[fInt[1] - 1], &vertices[fInt[0] - 1]));
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
}

void Mesh::FillBuffer(vector<Triangle3D*>& buffer)
{
    for (Triangle3D& tri : this->tris)
        buffer.push_back(&tri);
}

vector<Triangle3D> Mesh::GetTris() { return tris; }
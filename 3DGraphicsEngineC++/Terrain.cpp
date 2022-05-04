#include "Terrain.h"


Terrain::Terrain(Vector3D offset, int vertRows, int vertCols, int vertSpacing)
{
    // Create vertices
    for (int i = 0; i < vertRows; i++)
    {
        for (int j = 0; j < vertCols; j++)
        {
            vertices.push_back(Vector3D((i - (vertCols / 2) + offset.x) * vertSpacing, offset.y, (j - (vertRows / 2) + offset.z) * vertSpacing));
        }
    }
    
    // Create triangles w/ vertices
    for (size_t i = 0; i < vertRows - 1; i++)
    {
        for (size_t j = 0; j < vertCols - 1; j++)
        {
            tris.push_back(Triangle3D(&vertices[j + (i * vertRows)], &vertices[j + (i * vertRows) + vertCols], &vertices[j + (i * vertRows) + 1]));
            tris.push_back(Triangle3D(&vertices[j + (i * vertRows) + 1], &vertices[j + (i * vertRows) + vertCols], &vertices[j + (i * vertRows) + vertCols + 1]));
        }
    }
}

void Terrain::FillBuffer(vector<Triangle3D*>& buffer)
{
    for (Triangle3D& tri : this->tris)
        buffer.push_back(&tri);
}

vector<Vector3D>& Terrain::GetVertices()
{
    return this->vertices;
}
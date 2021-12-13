//
// Created by 闻永言 on 2021/8/7.
//

#include "Model.hpp"

using namespace std;

void Mesh::addVertex(Vertex &vertex)
{
    vertices.push_back(vertex);
    numVertices++;
}

void Mesh::addTriangle(Triangle &triangle)
{
    triangles.push_back(triangle);
    numTriangles++;
}

void Mesh::createBVH()
{
    if (!BVH)
    {
        BVH = new BVHNode();
        vector<Primitive *> primitives;
        for (auto &tri: triangles)
        {
            primitives.push_back(&tri);
        }

        BVH->create(primitives, 0);
    }
}

int Model::numMeshes() const
{
    return meshes.size();
}

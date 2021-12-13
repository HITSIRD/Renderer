//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_MODEL_HPP
#define RENDERER_MODEL_HPP

#include "Vertex.hpp"
#include "Triangle.hpp"
#include "BVH.hpp"
#include <vector>

class Material;

class Mesh
{
public:
    uint32_t numVertices;
    uint32_t numTriangles;

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Material *material;
    BVHNode *BVH;

    Mesh():numVertices(0), numTriangles(0), material(nullptr), BVH(nullptr){}

    ~Mesh()
    {
//        delete material;
        delete BVH;
    }

    /**
     *
     * @param vertex
     */
    void addVertex(Vertex &vertex);

    /**
     *
     * @param triangle
     */
    void addTriangle(Triangle &triangle);

    /**
     *
     */
    void createBVH();
};

class Model
{
public:
    std::vector<Mesh *> meshes;

    Model(){}

    ~Model()
    {
        for (auto mesh: meshes)
        {
            delete mesh;
        }
    }

    /**
     *
     * @return
     */
    int numMeshes() const;
};

#endif //RENDERER_MODEL_HPP

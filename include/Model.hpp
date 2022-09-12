//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_MODEL_HPP
#define RENDERER_MODEL_HPP

#include "Vertex.hpp"
#include "BVH.hpp"
#include <vector>

class Material;

class Triangle;

class Mesh
{
public:
    uint32_t numVertices;
    uint32_t numTriangles;

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    float4x4 matrixModel; // model space to world space matrix
    Material *material;
    BVHNode *BVH;

    Mesh();

    ~Mesh();

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
    BVHNode *BVH; // scene BVH root node

    Model();

    ~Model();

    /**
     *
     */
    void createBVH();

    /**
     *
     * @return
     */
    int numMeshes() const;
};

#endif //RENDERER_MODEL_HPP

//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_MODEL_HPP
#define RENDERER_MODEL_HPP

#include "Vertex.hpp"
#include "Triangle.hpp"
#include "Material.hpp"

class Mesh
{
public:
    uint32_t num_vertex;
    uint32_t num_triangle;

    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Material *material;

    Mesh():num_vertex(0), num_triangle(0), material(nullptr){}

    ~Mesh()
    {
        delete material;
        material = nullptr;
    }

    /**
     *
     * @param vertex
     */
    void add_vertex(Vertex &vertex);

    /**
     *
     * @param triangle
     */
    void add_triangle(Triangle &triangle);
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
            mesh->~Mesh();
        }
    }

    /**
     *
     * @return
     */
    int num_model() const;
};

#endif //RENDERER_MODEL_HPP

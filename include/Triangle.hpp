//
// Created by 闻永言 on 2021/7/11.
//

#ifndef RENDERER_TRIANGLE_HPP
#define RENDERER_TRIANGLE_HPP

#include "Vertex.hpp"
#include "Primitive.hpp"

class Mesh;

class Triangle: public Primitive
{
public:
    uint32_t vertexIndex[3]; // vertices index
    //    float4 normal; // Normal vector of triangle plane
    Mesh *mesh;

    Triangle();

    /**
     *
     * @param vertices
     * @param v0
     * @param v1
     * @param v2
     */
    Triangle(const std::vector<Vertex> &vertices, uint32_t v0, uint32_t v1, uint32_t v2);

    ~Triangle() override;

    /**
     *
     * @param ray
     * @param tMin
     * @param tMax
     * @param record
     * @return
     */
    bool hit(Ray &ray, float tMin, float tMax, HitRecord &record) override;

    /**
     * Destroy the leaf node. For a triangle, do nothing.
     */
    void destroy() override;
};

#endif

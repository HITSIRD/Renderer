//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_MESH_HPP
#define RENDERER_MESH_HPP

#include <vector>
#include "Eigen/Core"
#include "Triangle.hpp"

class Mesh
{
public:
    uint32_t num_vertex;
    uint32_t num_triangle;

    std::vector<Eigen::Vector3f> vertices;
    std::vector<Triangle> triangles;

    Mesh(){
        num_vertex = 0;
        num_triangle = 0;
    }

    /**
     *
     * @param vertex
     */
    void add_vertex(Eigen::Vector3f &vertex);

    /**
     *
     * @param triangle
     */
    void add_triangle(Triangle &triangle);
};

#endif //RENDERER_MESH_HPP

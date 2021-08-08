//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_MESH_HPP
#define RENDERER_MESH_HPP

#include <vector>
#include "Eigen/Dense"
#include "Triangle.hpp"

class Mesh
{
public:
    uint32_t num_vertex;
    uint32_t num_triangles;

    std::vector<Eigen::Vector3d> vertices;
    std::vector<Triangle> triangles;

    Mesh(){
        num_vertex = 0;
        num_triangles = 0;
    }

    /**
     *
     * @param vertex
     */
    void add_vertex(Eigen::Vector3d &vertex);

    /**
     *
     * @param triangle
     */
    void add_triangle(Triangle &triangle);
};

#endif //RENDERER_MESH_HPP

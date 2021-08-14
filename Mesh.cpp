//
// Created by 闻永言 on 2021/8/7.
//

#include "Mesh.hpp"

void Mesh::add_vertex(Eigen::Vector3f &vertex){
    vertices.push_back(vertex);
    num_vertex++;
}

void Mesh::add_triangle(Triangle &triangle){
    triangles.push_back(triangle);
    num_triangle++;
}

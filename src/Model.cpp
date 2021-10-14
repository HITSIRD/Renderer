//
// Created by 闻永言 on 2021/8/7.
//

#include "Model.hpp"

void Mesh::add_vertex(Vertex &vertex){
    vertices.push_back(vertex);
    num_vertex++;
}

void Mesh::add_triangle(Triangle &triangle){
    triangles.push_back(triangle);
    num_triangle++;
}

int Model::num_model() const{
    return meshes.size();
}

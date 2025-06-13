//
// Created by 闻永言 on 2021/8/7.
//

#include "Model.hpp"
#include "Material.hpp"

using namespace std;

Mesh::Mesh(): numVertices(0), numTriangles(0), material(nullptr), BVH(nullptr) {
}

Mesh::~Mesh() {
    delete BVH;
    delete material;
}

void Mesh::addVertex(Vertex &vertex) {
    vertices.push_back(vertex);
    numVertices++;
}

void Mesh::addTriangle(Triangle &triangle) {
    triangles.push_back(triangle);
    numTriangles++;
}

void Mesh::createBVH() {
    if (!BVH) {
        vector<Primitive *> primitives;
        for (auto &tri: triangles) {
            primitives.push_back(&tri);
        }
        BVH = new BVHNode(primitives, 0);
    }
}

Model::Model(): BVH(nullptr) {
}

Model::~Model() {
    for (auto mesh: meshes) {
        delete mesh;
    }
}

void Model::createBVH() {
    if (!BVH) {
        vector<Primitive *> primitives;
        for (const auto mesh: meshes) {
            mesh->createBVH();
            primitives.push_back(mesh->BVH);
        }
        BVH = new BVHNode(primitives, 0);
    }
}

int Model::numMeshes() const {
    return meshes.size();
}

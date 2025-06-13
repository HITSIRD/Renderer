//
// Created by 闻永言 on 2021/12/7.
//

#include "Triangle.hpp"
#include "Eigen/Geometry"
#include "Util.hpp"
#include "Model.hpp"
#include <vector>

using namespace std;
using namespace Renderer;

Triangle::Triangle() = default;

Triangle::Triangle(const std::vector<Vertex> &vertices, uint32_t v_0, uint32_t v_1, uint32_t v_2) {
    vertexIndex[0] = v_0;
    vertexIndex[1] = v_1;
    vertexIndex[2] = v_2;
    const float4 &p_0 = vertices[v_0].position;
    const float4 &p_1 = vertices[v_1].position;
    const float4 &p_2 = vertices[v_2].position;

    float4 center4 = (vertices[v_0].position + vertices[v_1].position + vertices[v_2].position) / 3.0f;
    center << center4.x(), center4.y(), center4.z();
    float x_min = min(min(p_0.x(), p_1.x()), p_2.x());
    float x_max = max(max(p_0.x(), p_1.x()), p_2.x());
    float y_min = min(min(p_0.y(), p_1.y()), p_2.y());
    float y_max = max(max(p_0.y(), p_1.y()), p_2.y());
    float z_min = min(min(p_0.z(), p_1.z()), p_2.z());
    float z_max = max(max(p_0.z(), p_1.z()), p_2.z());
    float bound[6] = {x_min, x_max, y_min, y_max, z_min, z_max};
    box = BoundingBox(bound);
}

Triangle::~Triangle() = default;

bool Triangle::hit(Ray &ray, float tMin, float tMax, HitRecord &record) {
    const Vertex &v0 = mesh->vertices[vertexIndex[0]];
    const Vertex &v1 = mesh->vertices[vertexIndex[1]];
    const Vertex &v2 = mesh->vertices[vertexIndex[2]];

    float4 E1 = v1.position - v0.position;
    float4 E2 = v2.position - v0.position;
    float4 P = ray.direction.cross3(E2);
    float det = E1.dot(P);

    // keep det > 0, modify T accordingly
    float4 T;
    if (det > 0) {
        T = ray.origin - v0.position;
    } else {
        T = v0.position - ray.origin;
        det = -det;
    }

    // If determinant is near zero, ray lies in plane of triangle
    if (det < 0.00000012f) {
        return false;
    }

    float4 Q = T.cross3(E1);
    // Calculate t, scale parameters, ray intersects triangle
    float t = E2.dot(Q);

    if (t < 0) {
        return false;
    }

    float v = T.dot(P);
    if (v < 0.0f || v > det) {
        return false;
    }

    float w = ray.direction.dot(Q);
    if (w < 0.0f || v + w > det) {
        return false;
    }

    float det_inv = 1.0f / det;
    t *= det_inv;
    v *= det_inv;
    w *= det_inv;

    record.t = t;
    record.material = mesh->material;
    record.color = lerp(v0.color, v1.color, v2.color, 1 - v - w, v, w);
    record.flatNormal = E1.cross3(E2).normalized();
    record.normal = lerp(v0.normal, v1.normal, v2.normal, 1 - v - w, v, w);
    record.tangent = lerp(v0.tangent, v1.tangent, v2.tangent, 1 - v - w, v, w);
    record.textureCoord = lerp(v0.textureCoord, v1.textureCoord, v2.textureCoord, 1 - v - w, v, w);
    return true;
}

void Triangle::destroy() {
    delete this;
}

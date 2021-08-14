//
// Created by 闻永言 on 2021/7/11.
//

#ifndef RENDERER_TRIANGLE_HPP
#define RENDERER_TRIANGLE_HPP

#include "Eigen/Core"

class Triangle
{
public:
    //    Eigen::Vector3f vertex_0;
    //    Eigen::Vector3f vertex_1;
    //    Eigen::Vector3f vertex_2;

    // Index of vertices
    uint32_t vertex_0;
    uint32_t vertex_1;
    uint32_t vertex_2;

    /**
     * Parameters of plane equation Ax + By + Cz + D = 0
     */
//    float A;
//    float B;
//    float C;
//    float D;

    /**
     * Normal vector of triangle plane
     */
    Eigen::Vector3f normal;

    /**
     * A point on plane
     */
//    Eigen::Vector3f plane_point;

    Triangle(std::vector<Eigen::Vector3f> &vertices, uint32_t v_0, uint32_t v_1, uint32_t v_2)
    {
        vertex_0 = v_0;
        vertex_1 = v_1;
        vertex_2 = v_2;
        float A = ((vertices[v_1].y() - vertices[v_0].y()) * (vertices[v_2].z() - vertices[v_0].z()) -
             (vertices[v_1].z() - vertices[v_0].z()) * (vertices[v_2].y() - vertices[v_0].y()));
        float B = ((vertices[v_1].z() - vertices[v_0].z()) * (vertices[v_2].x() - vertices[v_0].x()) -
             (vertices[v_1].x() - vertices[v_0].x()) * (vertices[v_2].z() - vertices[v_0].z()));
        float C = ((vertices[v_1].x() - vertices[v_0].x()) * (vertices[v_2].y() - vertices[v_0].y()) -
             (vertices[v_1].y() - vertices[v_0].y()) * (vertices[v_2].x() - vertices[v_0].x()));
//        D = -(A * vertices[v_0].x() + B * vertices[v_0].y() + C * vertices[v_0].z());

//        plane_point = vertices[v_0];
        normal << A, B, C;
        normal /= normal.norm(); // normalize
    };
};

#endif

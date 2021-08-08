//
// Created by 闻永言 on 2021/7/11.
//

#ifndef RENDERER_TRIANGLE_HPP
#define RENDERER_TRIANGLE_HPP

#include "Eigen/Dense"

class Triangle
{
public:
    //    Eigen::Vector3d vertex_0;
    //    Eigen::Vector3d vertex_1;
    //    Eigen::Vector3d vertex_2;

    // Index of vertices
    uint32_t vertex_0;
    uint32_t vertex_1;
    uint32_t vertex_2;

//    /**
//     * Parameters of plane equation Ax + By + Cz + D = 0
//     */
//    double A;
//    double B;
//    double C;
//    double D;

    /**
     * Normal vector of triangle plane
     */
    Eigen::Vector3d normal;

    Triangle(std::vector<Eigen::Vector3d> &vertices, uint32_t v_0, uint32_t v_1, uint32_t v_2)
    {
        vertex_0 = v_0;
        vertex_1 = v_1;
        vertex_2 = v_2;
        double A = ((vertices[v_1].y() - vertices[v_0].y()) * (vertices[v_2].z() - vertices[v_0].z()) -
             (vertices[v_1].z() - vertices[v_0].z()) * (vertices[v_2].y() - vertices[v_0].y()));
        double B = ((vertices[v_1].z() - vertices[v_0].z()) * (vertices[v_2].x() - vertices[v_0].x()) -
             (vertices[v_1].x() - vertices[v_0].x()) * (vertices[v_2].z() - vertices[v_0].z()));
        double C = ((vertices[v_1].x() - vertices[v_0].x()) * (vertices[v_2].y() - vertices[v_0].y()) -
             (vertices[v_1].y() - vertices[v_0].y()) * (vertices[v_2].x() - vertices[v_0].x()));
//        D = -(A * vertices[v_0].x() + B * vertices[v_0].y() + C * vertices[v_0].z());


        normal << A, B, C;
    };
};

#endif
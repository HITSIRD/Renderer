//
// Created by 闻永言 on 2021/7/11.
//

#ifndef DEPTH_SEARCH_TRIANGLE_HPP
#define DEPTH_SEARCH_TRIANGLE_HPP

#endif //DEPTH_SEARCH_TRIANGLE_HPP

#include "Eigen/Dense"

class Triangle
{
public:
    Eigen::Vector3d vertex_0;
    Eigen::Vector3d vertex_1;
    Eigen::Vector3d vertex_2;

    /**
     * Parameters of plane equation Ax + By + Cz + D = 0
     */
    double A;
    double B;
    double C;
    double D;

    Eigen::Vector3d normal;

    Triangle(
            Eigen::Vector3d v_0, Eigen::Vector3d v_1, Eigen::Vector3d v_2)
    {
        vertex_0 = v_0;
        vertex_1 = v_1;
        vertex_2 = v_2;
        A = ((v_1.y() - v_0.y()) * (v_2.z() - v_0.z()) - (v_1.z() - v_0.z()) * (v_2.y() - v_0.y()));
        B = ((v_1.z() - v_0.z()) * (v_2.x() - v_0.x()) - (v_1.x() - v_0.x()) * (v_2.z() - v_0.z()));
        C = ((v_1.x() - v_0.x()) * (v_2.y() - v_0.y()) - (v_1.y() - v_0.y()) * (v_2.x() - v_0.x()));
        D = -(A * v_0.x() + B * v_0.y() + C * v_0.z());

        normal << A, B, C;
    };
};
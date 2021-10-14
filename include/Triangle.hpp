//
// Created by 闻永言 on 2021/7/11.
//

#ifndef RENDERER_TRIANGLE_HPP
#define RENDERER_TRIANGLE_HPP

#include "Vertex.hpp"
#include "Eigen/Geometry"
#include <iostream>
#include <vector>

using namespace std;

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

    float4 normal; // Normal vector of triangle plane

//    bool is_clip; // if is clipped

    /**
     * A world on plane
     */
    //    Eigen::Vector3f plane_point;

    Triangle(std::vector<Vertex> &vertices, uint32_t v_0, uint32_t v_1, uint32_t v_2)
    {
//        is_clip = false;
        vertex_0 = v_0;
        vertex_1 = v_1;
        vertex_2 = v_2;
        float4 OA = vertices[v_1].world - vertices[v_0].world;
        float4 OB = vertices[v_2].world - vertices[v_0].world;

        normal << OA.cross3(OB);
        //        cout << OA.cross(OB) << endl;
        //                normal << A, B, C, 0;
        normal.normalize(); // normalize
    };
};

#endif

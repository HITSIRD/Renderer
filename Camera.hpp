//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_CAMERA_HPP
#define RENDERER_CAMERA_HPP

#include "Type.hpp"
#include "Eigen/Geometry"

class Camera
{
public:
    // camera inner parameters
    int pixel_x; // pixel number in x
    int pixel_y; // pixel number in y
    float ccd_size_x; // (mm)
    float ccd_size_y; // (mm)
    float f; // (mm)

    // c4d world coordinate parameters
    float x;
    float y;
    float z;
    float h;
    float p;
    float b;

    Mat3 K;
    Mat3 R3;
    Eigen::Vector3f t3;
    Vec3 d;

//    Eigen::Matrix3f K_inv;
//    Eigen::Matrix3f R_inv;

    Mat4 R;
    Mat4 t;
    Mat4 VM; // world to view matrix
    Mat34 SM; // view to screen matrix
    Mat4 Q; // normal vector transform matrix
    Mat34 OM; // orthographic transform matrix

//    Eigen::Vector3f center;

    /**
     * constructor
     *
     * @param ccd_size_x ccd size in x
     * @param ccd_size_y ccd size in y
     * @param f focal of camera
     * @param x
     * @param y
     * @param z
     * @param h
     * @param p
     * @param b
     */
    void read_param(
            int pixel_x, int pixel_y, float ccd_size_x, float ccd_size_y, float f, float x, float y, float z,
            float h, float p, float b);

    /**
     * destructor
     */
    ~Camera();

    /**
     * Convert the c4d left-handed world coordinate to camera right-handed coordinate, write_result the inner matrix and outer matrix.
     */
    void convert();

private:
    /**
    * Calculate rotation matrix of H, P, B.
    * @param H
    * @param P
    * @param B
    */
    void calculate_HPB(Mat3 &H, Mat3 &P, Mat3 &B);

    /**
     * Calculate inner matrix OM.
     */
    void calculate_K();

    /**
     * Calculate rotation matrix R3.
     */
    void calculate_R();

    /**
     * Calculate translation vector t3.
     */
    void calculate_t();

    /**
     * Convert deg to rad.
     * @param deg
     * @return rad
     */
    static float deg2rad(float deg);
};

#endif

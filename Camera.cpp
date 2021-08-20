//
// Created by 闻永言 on 2021/7/10.
//

#include "Camera.hpp"

using namespace Eigen;

void Camera::read_param(
        int pixel_x, int pixel_y, float ccd_size_x, float ccd_size_y, float f, float x, float y, float z, float h,
        float p, float b)
{
    Camera::pixel_x = pixel_x;
    Camera::pixel_y = pixel_y, Camera::ccd_size_x = ccd_size_x;
    Camera::ccd_size_y = ccd_size_y;
    Camera::f = f;
    Camera::x = x;
    Camera::y = y;
    Camera::z = z;
    Camera::h = h;
    Camera::p = p;
    Camera::b = b;
    //    Camera::center << x / 100.0f, -y / 100.0f, z / 100.0f; // left system to right system
}

Camera::~Camera() = default;

void Camera::convert()
{
    Mat3 H, P, B;
    calculate_HPB(H, P, B);
    calculate_K();
    calculate_R();
    calculate_t();
    RowVector4f term_0(0, 0, 0, 1);
    VM << R3, t3, term_0;
    Vector3f term_1(0, 0, 0);
    SM << K, term_1;
    R << R3, term_1, term_0;
    Matrix3f m;
    m.setZero();
    t << m, t3, term_0;
    Q = VM.inverse().transpose();
    OM << (float)pixel_x / 310.0f, 0,                       0, float(pixel_x) / 2.f,
           0,                      float(pixel_y) / 310.0f, 0, float(pixel_y) / 2.f,
           0,                      0,                       1, 0;
}

void Camera::calculate_HPB(Mat3 &H, Mat3 &P, Mat3 &B)
{
    float h_rad = -deg2rad(h);
    float p_rad = -deg2rad(p);
    float b_rad = -deg2rad(b);

    H << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad);
    P << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad);
    B << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1;
}

void Camera::calculate_K()
{
    float f_x = (float)pixel_x / ccd_size_x * f;
    float f_y = (float)pixel_y / ccd_size_y * f;
    float c_x = (float)pixel_x / 2;
    float c_y = (float)pixel_y / 2;

    K << f_x, 0, c_x, 0, f_y, c_y, 0, 0, 1;
}

void Camera::calculate_R()
{
    Mat3 H, P, B, S_y;
    calculate_HPB(H, P, B);
    S_y << 1, 0, 0, 0, -1, 0, 0, 0, 1;
    R3 = S_y * H * P * B * S_y;
    R3.transposeInPlace();
}

void Camera::calculate_t()
{
    Vector3f w_c(x / 100.0f, -y / 100.0f, z / 100.0f); // cm to m and left to right system
    t3 = -(R3 * w_c);
}

float Camera::deg2rad(float deg)
{
    return float(deg * atan(1.0) / 45.0);
}

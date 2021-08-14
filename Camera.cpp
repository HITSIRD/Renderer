//
// Created by 闻永言 on 2021/7/10.
//

#include "Camera.hpp"

using namespace Eigen;

void Camera::read_param(
        int pixel_x, int pixel_y, float ccd_size_x, float ccd_size_y, float f, float x, float y, float z,
        float h, float p, float b)
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
    Camera::center << x / 100.0f, -y / 100.0f, z / 100.0f; // left system to right system
}

Camera::~Camera() = default;

void Camera::calculate()
{
    Matrix3f H, P, B;
    calculate_HPB(H, P, B);
    calculate_K();
    calculate_R();
    calculate_t();
}

//void Camera::calculate_HPB(cv::Mat &H, cv::Mat &P, cv::Mat &B)
//{
//    float h_rad = -deg2rad(h);
//    float p_rad = -deg2rad(p);
//    float b_rad = -deg2rad(b);
//
//    H = (cv::Mat_<float >(3, 3) << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad));
//    P = (cv::Mat_<float >(3, 3) << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad));
//    B = (cv::Mat_<float >(3, 3) << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1);
//};

void Camera::calculate_HPB(Matrix3f &H, Matrix3f &P, Matrix3f &B)
{
    float h_rad = -deg2rad(h);
    float p_rad = -deg2rad(p);
    float b_rad = -deg2rad(b);

    H << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad);
    P << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad);
    B << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1;
}

//void Camera::calculate_K()
//{
//    float f_x = (float )pixel_x / ccd_size_x * f;
//    float f_y = (float )pixel_y / ccd_size_y * f;
//    float c_x = (float )pixel_x / 2;
//    float c_y = (float )pixel_y / 2;
//
//    K = (cv::Mat_<float >(3, 3) << f_x, 0, c_x, 0, f_y, c_y, 0, 0, 1);
//    cv::invert(K, K_inv);
//};

void Camera::calculate_K()
{
    float f_x = (float )pixel_x / ccd_size_x * f;
    float f_y = (float )pixel_y / ccd_size_y * f;
    float c_x = (float )pixel_x / 2;
    float c_y = (float )pixel_y / 2;

    K << f_x, 0, c_x, 0, f_y, c_y, 0, 0, 1;
    K_inv = K.inverse();
}

//void Camera::calculate_R()
//{
//    cv::Mat H, P, B;
//    calculate_HPB(H, P, B);
//    cv::Mat S_y = (cv::Mat_<float >(3, 3) << 1, 0, 0, 0, -1, 0, 0, 0, 1);
//    R = S_y * H * P * B * S_y;
//    cv::transpose(R, R);
//    cv::invert(R, R_inv);
//};

void Camera::calculate_R()
{
    Matrix3f H, P, B, S_y;
    calculate_HPB(H, P, B);
    S_y << 1, 0, 0, 0, -1, 0, 0, 0, 1;
    R = S_y * H * P * B * S_y;
    R.transposeInPlace();
    R_inv = R.inverse();
}

//void Camera::calculate_t()
//{
//    cv::Mat w_c = (cv::Mat_<float >(3, 1) << x / 100.0, -y / 100.0, z / 100.0);// cm to m and left to right system
//    t = -(R * w_c);
//}

void Camera::calculate_t()
{
    Vector3f w_c(x / 100.0f, -y / 100.0f, z / 100.0f); // cm to m and left to right system
    t = -(R * w_c);
}

float Camera::deg2rad(float deg)
{
    return float(deg * atan(1.0) / 45.0);
}

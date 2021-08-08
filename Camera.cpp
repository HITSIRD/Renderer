//
// Created by 闻永言 on 2021/7/10.
//

#include "Camera.hpp"

using namespace Eigen;

void Camera::read_param(
        int pixel_x, int pixel_y, double ccd_size_x, double ccd_size_y, double f, double x, double y, double z,
        double h, double p, double b)
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
    Camera::center << x / 100.0, -y / 100.0, z / 100.0; // left system to right system
};

Camera::~Camera() = default;

void Camera::calculate()
{
    Matrix3d H, P, B;
    calculate_HPB(H, P, B);
    calculate_K();
    calculate_R();
    calculate_t();
}

//void Camera::calculate_HPB(cv::Mat &H, cv::Mat &P, cv::Mat &B)
//{
//    double h_rad = -deg2rad(h);
//    double p_rad = -deg2rad(p);
//    double b_rad = -deg2rad(b);
//
//    H = (cv::Mat_<double>(3, 3) << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad));
//    P = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad));
//    B = (cv::Mat_<double>(3, 3) << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1);
//};

void Camera::calculate_HPB(Matrix3d &H, Matrix3d &P, Matrix3d &B)
{
    double h_rad = -deg2rad(h);
    double p_rad = -deg2rad(p);
    double b_rad = -deg2rad(b);

    H << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad);
    P << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad);
    B << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1;
};

//void Camera::calculate_K()
//{
//    double f_x = (double)pixel_x / ccd_size_x * f;
//    double f_y = (double)pixel_y / ccd_size_y * f;
//    double c_x = (double)pixel_x / 2;
//    double c_y = (double)pixel_y / 2;
//
//    K = (cv::Mat_<double>(3, 3) << f_x, 0, c_x, 0, f_y, c_y, 0, 0, 1);
//    cv::invert(K, K_inv);
//};

void Camera::calculate_K()
{
    double f_x = (double)pixel_x / ccd_size_x * f;
    double f_y = (double)pixel_y / ccd_size_y * f;
    double c_x = (double)pixel_x / 2;
    double c_y = (double)pixel_y / 2;

    K << f_x, 0, c_x, 0, f_y, c_y, 0, 0, 1;
    K_inv = K.inverse();
};

//void Camera::calculate_R()
//{
//    cv::Mat H, P, B;
//    calculate_HPB(H, P, B);
//    cv::Mat S_y = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, -1, 0, 0, 0, 1);
//    R = S_y * H * P * B * S_y;
//    cv::transpose(R, R);
//    cv::invert(R, R_inv);
//};

void Camera::calculate_R()
{
    Matrix3d H, P, B, S_y;
    calculate_HPB(H, P, B);
    S_y << 1, 0, 0, 0, -1, 0, 0, 0, 1;
    R = S_y * H * P * B * S_y;
    R.transposeInPlace();
    R_inv = R.inverse();
};

//void Camera::calculate_t()
//{
//    cv::Mat w_c = (cv::Mat_<double>(3, 1) << x / 100.0, -y / 100.0, z / 100.0);// cm to m and left to right system
//    t = -(R * w_c);
//};

void Camera::calculate_t()
{
    Vector3d w_c(x / 100.0, -y / 100.0, z / 100.0);// cm to m and left to right system
    t = -(R * w_c);
};

double Camera::deg2rad(double deg)
{
    return deg * atan(1.0) / 45.0;
}

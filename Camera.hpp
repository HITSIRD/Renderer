//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_CAMERA_HPP
#define RENDERER_CAMERA_HPP

#include "opencv2/highgui.hpp"
#include "Eigen/Core"
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

    //    cv::Mat K = cv::Mat::zeros(3, 3, CV_64F);
    //    cv::Mat R = cv::Mat::zeros(3, 3, CV_64F);
    //    cv::Mat t = cv::Mat::zeros(3, 1, CV_64F);
    //    cv::Mat d = cv::Mat::zeros(4, 1, CV_64F);

    Eigen::Matrix3f K;
    Eigen::Matrix3f R;
    Eigen::Vector3f t;
    Eigen::Vector4f d;

    //    cv::Mat K_inv;
    //    cv::Mat R_inv;

    Eigen::Matrix3f K_inv;
    Eigen::Matrix3f R_inv;

    Eigen::Vector3f center;

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
     * Convert the c4d left-handed world coordinate to camera right-handed coordinate, render the inner matrix and outer matrix.
     */
    void calculate();

private:
    //    /**
    //    * Calculate rotation matrix of H, P, B.
    //    * @param H
    //    * @param P
    //    * @param B
    //    */
    //    void calculate_HPB(cv::Mat &H, cv::Mat &P, cv::Mat &B);

    /**
    * Calculate rotation matrix of H, P, B.
    * @param H
    * @param P
    * @param B
    */
    void calculate_HPB(Eigen::Matrix3f &H, Eigen::Matrix3f &P, Eigen::Matrix3f &B);

    /**
     * Calculate inner matrix K.
     */
    void calculate_K();

    /**
     * Calculate rotation matrix R.
     */
    void calculate_R();

    /**
     * Calculate translation vector t.
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
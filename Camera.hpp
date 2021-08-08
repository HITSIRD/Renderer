//
// Created by 闻永言 on 2021/7/10.
//

#ifndef DEPTH_SEARCH_CAMERA_HPP
#define DEPTH_SEARCH_CAMERA_HPP

#include "opencv2/highgui.hpp"
#include "Eigen/Dense"

class Camera
{
public:
    // camera inner parameters
    int pixel_x; // pixel number in x
    int pixel_y; // pixel number in y
    double ccd_size_x; // (mm)
    double ccd_size_y; // (mm)
    double f; // (mm)

    // c4d world coordinate parameters
    double x;
    double y;
    double z;
    double h;
    double p;
    double b;

    //    cv::Mat K = cv::Mat::zeros(3, 3, CV_64F);
    //    cv::Mat R = cv::Mat::zeros(3, 3, CV_64F);
    //    cv::Mat t = cv::Mat::zeros(3, 1, CV_64F);
    //    cv::Mat d = cv::Mat::zeros(4, 1, CV_64F);

    Eigen::Matrix3d K;
    Eigen::Matrix3d R;
    Eigen::Vector3d t;
    Eigen::Vector4d d;

    //    cv::Mat K_inv;
    //    cv::Mat R_inv;

    Eigen::Matrix3d K_inv;
    Eigen::Matrix3d R_inv;

    Eigen::Vector3d center;

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
            int pixel_x, int pixel_y, double ccd_size_x, double ccd_size_y, double f, double x, double y, double z,
            double h, double p, double b);

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
    void calculate_HPB(Eigen::Matrix3d &H, Eigen::Matrix3d &P, Eigen::Matrix3d &B);

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
    double deg2rad(double deg);

};

#endif //DEPTH_SEARCH_CAMERA_HPP

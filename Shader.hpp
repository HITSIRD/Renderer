//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_SHADER_HPP
#define RENDERER_SHADER_HPP

#include "Eigen/Dense"
#include "Camera.hpp"

#define FLAT_SHADING 1
#define PHONG_SHADING 2

class Shader
{
public:
    double ka; // ambient coefficient
    double kd; // diffuse coefficient
    double ks; // specular coefficient

    double spec_rank; // power of specular coefficient

    Eigen::Vector3d light;
    Eigen::Vector3d view; // camera view vector

    Camera *camera;

    /**
     * Convert the light vector to view space.
     */
    void calibrate();

public:
    Shader(Camera *cam)
    {
        camera = cam;
    };

    /**
         *
         * @param config_file
         */
    void read_config(std::string &config_file);

    /**
     *
     * @param normal
     * @param point
     */
    void flat_shading(Eigen::Vector3d normal, double *point);

    /**
     *
     * @param normal_0
     * @param normal_1
     * @param normal_2
     * @param u
     * @param v
     * @param w
     * @param point
     */
    void phong_shading(
            Eigen::Vector3d &normal_0, Eigen::Vector3d &normal_1, Eigen::Vector3d &normal_2, double u, double v,
            double w, double *point);

    /**
     *
     * @param normal
     * @param point
     */
    void phong_shading(Eigen::Vector3d &normal, double *point);
};

#endif
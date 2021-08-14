//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_SHADER_HPP
#define RENDERER_SHADER_HPP

#include "Eigen/Core"
#include "Camera.hpp"
#include "Mesh.hpp"

#define FLAT_SHADING 1
#define PHONG_SHADING 2

#define EPSILON 0.5f

class Shader
{
    float ka; // ambient coefficient
    float kd; // diffuse coefficient
    float ks; // specular coefficient

    float spec_rank; // power of specular coefficient

    Eigen::Vector3f light;
    Eigen::Vector3f view; // camera view vector

    Camera *camera;
    Camera *light_view;

    float *shadow_buffer;
    const float max_depth = 1000.0f;

    Eigen::Matrix3f K; // orthographic matrix
    Eigen::Vector3f offset;

    /**
     * Convert the light vector to view space.
     */
    void calibrate();

    /**
     *
     * @param point
     * @param pixel
     */
    void pers_projection(Eigen::Vector3f point, Eigen::Vector2f &pixel);

    /**
     *
     * @param point
     * @param pixel
     */
    void ortho_projection(Eigen::Vector3f point, Eigen::Vector2f &pixel);

    /**
     *
     * @param pixel
     * @param view
     */
    //    void pixel_to_view(Eigen::Vector2f pixel, Eigen::Vector3f &view) const;

    /**
     *
     * @param triangle
     * @param p
     * @param cross
     * @return
     */
    //    float get_depth(Triangle triangle, Eigen::Vector2f p, Eigen::Vector3f &cross);
public:
    /**
     *
     * @param c
     */
    Shader(Camera *c)
    {
        camera = c;
        //        shadow_buffer = new float[raster->]
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
    void flat_shading(Eigen::Vector3f normal, float *point);

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
            Eigen::Vector3f &normal_0, Eigen::Vector3f &normal_1, Eigen::Vector3f &normal_2, float u, float v, float w,
            float *point);

    /**
     *
     * @param normal
     * @param point
     */
    //    void phong_shading(Eigen::Vector3f &normal, float *point);

    /**
     * Shadow mapping
     * @param triangle
     * @param pixel pixel center coordinate
     * @param point
     */
    //    void shadow_mapping(Triangle triangle, Eigen::Vector2f pixel, float *point);

    /**
     *
     * @param vertex_0
     * @param vertex_1
     * @param vertex_2
     * @param u
     * @param v
     * @param w
     * @param point
     */
    void shadow_mapping(
            Eigen::Vector3f &vertex_0, Eigen::Vector3f &vertex_1, Eigen::Vector3f &vertex_2, float u, float v, float w,
            float *point);

    /**
     * Calculate the shadow map.
     * @param mesh
     */
    void shadow_map(Mesh *mesh);

    void write_depth_image(float *z_buffer);

    //    void get_cross_point(Triangle triangle, Eigen::Vector3f P, Eigen::Vector3f &cross);
};

#endif

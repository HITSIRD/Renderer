//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_RASTERIZATION_HPP
#define RENDERER_RASTERIZATION_HPP

#include "Eigen/Core"
#include "Convert.hpp"
#include "iodata.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

#define GAMMA 1/2.2

typedef struct pixel
{
    float d; //depth
    float l;
} Pixel;

class Rasterization
{
public:
    /**
     * Constructor
     * @param pixel_x
     * @param pixel_y
     * @param D_x
     * @param D_y
     * @param c // camera
     * @param s // shading type
     * @return
     */
    Rasterization(uint16_t pixel_x, uint16_t pixel_y, uint16_t D_x, uint16_t D_y, Camera *c, float max_depth, int s)
    {
        x = pixel_x;
        y = pixel_y;
        DEM_x = D_x;
        DEM_y = D_y;
        z_buffer = new float[pixel_x * pixel_y];
        camera = c;
        shading_type = s;
        for (int i = 0; i < y; i++)
        {
            for (int j = 0; j < x; j++)
            {
                z_buffer[i * x + j] = max_depth;
            }
        }
    }

    /**
     *
     * @param mesh
     * @param shader
     * @param image_buffer
     */
    void rasterize(Mesh *mesh, Shader *shader, float *image_buffer);

private:
    uint16_t x; // output image resolution
    uint16_t y; // output image resolution
    uint16_t DEM_x; // DEM resolution
    uint16_t DEM_y; // DEM resolution
    float *z_buffer; // value in [0, 1], z = (point.d - min_depth)/(max_depth - min_depth)
    Camera *camera;
    int shading_type;

    /**
     * Calculate the pixel coordinate of 3f point by perspective projection.
     * @param point 3f point
     * @param pixel coordinate of back projection
     */
    void pers_projection(Eigen::Vector3f point, Eigen::Vector2f &pixel);

    /**
     * Get bounding box coordinate t_0(min(p)) and t_1(max(p))
     * @param p_0
     * @param p_1
     * @param p_2
     * @param t_0
     * @param t_1
     */
    static void get_bounding_box(
            Eigen::Vector2f p_0, Eigen::Vector2f p_1, Eigen::Vector2f p_2, Eigen::Vector2i &t_0, Eigen::Vector2i &t_1);

    /**
     * Detect if a 2f point is in a given triangle.
     * @param p_0
     * @param p_1
     * @param p_2
     * @param p
     * @param u
     * @param v
     * @param w
     * @return true if in triangle
     */
    static bool is_in_triangle(
            Eigen::Vector2f p_0, Eigen::Vector2f p_1, Eigen::Vector2f p_2, Eigen::Vector2f p, float &u, float &v,
            float &w);

    /**
     * Calculate the depth from optical center to point that is crossed by given point to plane.
     * @param triangle plane of triangle
     * @param p given pixel center
     * @return depth from optical center to point that is crossed by given point to plane
     */
    //    float get_depth(Triangle triangle, Eigen::Vector2f p);

    /**
     * Interpolate the depth of point in triangle.
     * @param d_0
     * @param d_1
     * @param d_2
     * @param u
     * @param v
     * @param w
     * @return depth from optical center to point that is crossed by given point to plane
     */
    static float interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w);

    /**
     * Calculate camera coordinate of given pixel coordinate.
     * @param pixel pixel coordinate in pixel system
     * @param world world coordinate of the pixel in world system
     */
    //    void pixel_to_view(Eigen::Vector2f pixel, Eigen::Vector3f &view) const;

    /**
     * Get the cross point in plane.
     * @param triangle plane of triangle
     * @param p world coordinate in world system
     * @param cross cross point in the plane A_0 A_1 A_2
     */
    //    void get_cross_point(Triangle triangle, Eigen::Vector3f P, Eigen::Vector3f &cross) const;
};

#endif

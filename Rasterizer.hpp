//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_RASTERIZER_HPP
#define RENDERER_RASTERIZER_HPP

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"

#define GAMMA 1/2.2

typedef struct pixel
{
    float d; //depth
    float l;
} Pixel;

class Rasterizer
{
public:
    /**
     * Constructor
     * @param pixel_x
     * @param pixel_y
     * @param c // camera
     * @return
     */
    Rasterizer(uint16_t pixel_x, uint16_t pixel_y, Camera *c, int type, float max_depth)
    {
        x = pixel_x;
        y = pixel_y;
        camera = c;
        shading_type = type;
        //        depth_x = pixel_x * multi_sample;
        //        depth_y = pixel_y * multi_sample;
        //        z_buffer = new float[depth_x * depth_y];
        //        for (int i = 0; i < depth_y; i++)
        //        {
        //            for (int j = 0; j < depth_x; j++)
        //            {
        //                z_buffer[i * depth_x + j] = max_depth;
        //            }
        //        }
    }

    /**
     * Rasterization.
     * @param mesh input triangles
     * @param frag_buffer fragment stream
     */
    void rasterize(Mesh *mesh, std::vector<Fragment> &frag_buffer);

private:
    uint16_t x; // output resolution
    uint16_t y; // output resolution
    //    uint16_t DEM_x; // DEM resolution
    //    uint16_t DEM_y; // DEM resolution
    //    uint32_t depth_x; // z_buffer resolution
    //    uint32_t depth_y; // z_buffer resolution
    //    float *z_buffer; // value in [0, 1], z = (point.d - min_depth)/(max_depth - min_depth)
    Camera *camera;
    int shading_type;
    //    uint8_t multi_sample = 4; // MSAA sampling rate like 2x2, 4x4, 8x8
    const float max_depth = 1000.0f; // clip max depth

    /**
     * Calculate the pixel coordinate of 3f point by perspective projection.
     * @param point 3f point
     * @param pixel coordinate of back projection
     */
    inline void pers_projection(Point3f point, Point2f &pixel);

    /**
     * Calculate the pixel coordinate of 3f point by orthographic projection.
     * @param point 3f point
     * @param pixel coordinate of back projection
     */
    void ortho_projection(Point3f point, Point2f &pixel);

    /**
     * Get bounding box coordinate t_0(min(p)) and t_1(max(p))
     * @param p_0
     * @param p_1
     * @param p_2
     * @param t_0
     * @param t_1
     */
    static void get_bounding_box(Point2f p_0, Point2f p_1, Point2f p_2, Point2i &t_0, Point2i &t_1);

    /**
     * Detect if a 2f point is in a given triangle and convert the core coordinate.
     * @param p_0 a vertex of 2d triangle
     * @param p_1 a vertex of 2d triangle
     * @param p_2 a vertex of 2d triangle
     * @param p
     * @param u
     * @param v
     * @param w
     * @return true if is in triangle
     */
    static bool is_in_triangle(
            Point2f p_0, Point2f p_1, Point2f p_2, Point2f p, float &u, float &v, float &w);

    /**
     * Detect if a 2f point is in a given triangle.
     * @param P_0 a vertex of 2d triangle
     * @param P_1 a vertex of 2d triangle
     * @param P_2 a vertex of 2d triangle
     * @param P
     * @return true if is in triangle
     */
    static bool is_in_triangle(Point2f P_0, Point2f P_1, Point2f P_2, Point2f P);

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
    inline static float interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w);
};

#endif

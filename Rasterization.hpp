//
// Created by 闻永言 on 2021/7/10.
//

#ifndef DEPTH_SEARCH_RASTERIZATION_HPP
#define DEPTH_SEARCH_RASTERIZATION_HPP

#include "opencv2/highgui.hpp"
#include "Eigen/Dense"
#include "convert.hpp"
#include "iodata.hpp"
#include "Triangle.hpp"
#include <fstream>
#include <ctime>
#include <sys/time.h>
#include <algorithm>

#define EPSILON 1e-9
#define GAMMA 1/2.2

typedef struct pixel
{
    double d; //depth
    double l;
} Pixel;

class Rasterization
{
public:
    int x; // resolution
    int y; // resolution
//    std::vector<Pixel> pixels;
//    std::vector<double> z_buffer;
    double* z_buffer;
    double* image_buffer;
    Camera *camera;
    std::vector<Eigen::Vector3d> mesh;
    std::vector<Triangle *> triangles;
    Eigen::Vector3d illuminant;
    cv::Mat image;
    uint16_t lut[65535]; // look up table

    /**
     *
     * @param dem_file
     * @param camera_file
     * @param illuminant_file
     */
    void read_data(std::string &dem_file, std::string &camera_file, std::string &illuminant_file);

    /**
     *
     */
    void initialize();

    /**
     *
     */
    void rasterize();

    /**
         * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
         */
    void write_result_depth_image();

private:
    const double max_depth = 10000;

    void set_pixel();

    /**
     * Adjust the order of vertex in every triangle.
     */
    void sort_vertex();

    /**
     * Calculate the pixel coordinate of 3d point.
     * @param point 3d point
     * @param pixel coordinate of back projection.
     */
    void back_projection(Eigen::Vector3d point, Eigen::Vector2d &pixel);

    /**
     * Get bounding box coordinate t_0(min(p)) and t_1(max(p))
     * @param p_0
     * @param p_1
     * @param p_2
     * @param t_0
     * @param t_1
     */
    void get_bounding_box(
            Eigen::Vector2d p_0, Eigen::Vector2d p_1, Eigen::Vector2d p_2, Eigen::Vector2i &t_0, Eigen::Vector2i &t_1);

    /**
     * Detect if a 2d point is in a given triangle.
     * @param p_0
     * @param p_1
     * @param p_2
     * @param p
     * @return true if in triangle
     */
    bool is_in_triangle(Eigen::Vector2d p_0, Eigen::Vector2d p_1, Eigen::Vector2d p_2, Eigen::Vector2d p);

    /**
     * Calculate the depth from optical center to point that is crossed by given point to plane.
     * @param triangle plane of triangle
     * @param p given pixel center
     * @return depth from optical center to point that is crossed by given point to plane
     */
    double get_depth(Triangle triangle, Eigen::Vector2d p);

    /**
     * Interpolate the depth of point in triangle.
     * @param triangle
     * @param p
     * @return depth from optical center to point that is crossed by given point to plane
     */
    double interpolate_depth(Triangle triangle, Eigen::Vector2d p);

    /**
     * Calculate world coordinate of given pixel coordinate.
     * @param pixel pixel coordinate in pixel system
     * @param world world coordinate of the pixel in world system
     */
    void pixel_to_world(Eigen::Vector2d pixel, Eigen::Vector3d &world) const;

    /**
     * Get the cross point in plane.
     * @param triangle plane of triangle
     * @param p world coordinate in world system
     * @param cross cross point in the plane A_0 A_1 A_2
     */
    void get_cross_point(Triangle triangle, Eigen::Vector3d P, Eigen::Vector3d &cross) const;

    /**
     *
     */
    void write_result_image();
};

#endif //DEPTH_SEARCH_RASTERIZATION_HPP

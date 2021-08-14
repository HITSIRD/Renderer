//
// Created by 闻永言 on 2021/8/8.
//

#ifndef RENDERER_RENDER_HPP
#define RENDERER_RENDER_HPP

#include "Rasterization.hpp"
#include "Shader.hpp"

#define EPSILON 1e-3
#define GAMMA 1/2.2

class Render
{
public:
    int x; // resolution
    int y; // resolution

    /**
     *
     * @param dem_file
     * @param camera_file
     */
    void read_data(std::string &dem_file, std::string &camera_file);

    /**
     *
     * @param config_file
     */
    void read_config(std::string &config_file);

    /**
     *
     * @param shader_ shading type
     */
    void initialize(int shader_);

    /**
     *
     */
    void rasterize();

    /**
     *
     * @return vertex number
     */
    uint32_t num_vertex();

    /**
     *
     * @return triangle number
     */
    uint32_t num_triangle();

    /**
     * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
     * @param z_buffer
     */
    void write_depth_image(float *z_buffer);

    /**
     *
     */
    void write_result_image();

private:
    uint16_t DEM_x; // DEM resolution
    uint16_t DEM_y; // DEM resolution
    float *image_buffer;
    cv::Mat image;
    uint16_t lut[65536]; // look up table
    int shading_type;

    Camera *camera;
    Mesh *mesh;

    Rasterization *raster;
    Shader *shader;

    const float min_depth = 10.0; // clip min depth
    const float max_depth = 1000.0; // clip max depth

    /**
     * Adjust the order of vertex in every triangle.
     */
    void sort_vertex();
};

#endif //RENDERER_RENDER_HPP

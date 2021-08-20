//
// Created by 闻永言 on 2021/8/8.
//

#ifndef RENDERER_RENDER_HPP
#define RENDERER_RENDER_HPP

#endif //RENDERER_RENDER_HPP

#include "Type.hpp"
#include "Rasterizer.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "VertexShader.hpp"
#include "FragmentShader.hpp"
#include "opencv2/highgui.hpp"
#include "Fragment.hpp"
#include "unordered_set"

#define GAMMA 1/2.2

class Renderer
{
public:
    int x; // output frame resolution
    int y; // output frame resolution

    /**
     *
     * @param dem_file
     * @param camera_file
     */
    void read_data(std::string &dem_file, std::string &camera_file);

    /**
     *
     * @param config_file
     * @param type shading type
     */
    void read_config(std::string &config_file, int type);

    /**
     *
     */
    void render();

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
     * @param depth_buffer
     * @param c
     */
    void write_depth_image(float *depth_buffer, Camera *c);

    /**
     *
     */
    void write_result_image();

private:
    uint16_t DEM_x; // DEM resolution
    uint16_t DEM_y; // DEM resolution
    int shading_type;
    uint16_t lut[65536]; // look up table

    Camera *camera;
    Camera *light_view; // to generate shadow map
    Mesh *mesh;

    Rasterizer *raster;
    VertexShader *vertex_shader;
    FragmentShader *fragment_shader;

    uint32_t num_frag; // number of fragments

    std::vector<Fragment> frag_buffer;
    float *shadow_map;
    float *stencil_buffer;
    float *z_buffer; // depth buffer
    bool *test_buffer; // test
    float *frame_buffer;
    cv::Mat image;

    int max_threads; // max thread number

    const float min_depth = 10.0f; // clip min depth
    const float max_depth = 1000.0f; // clip max depth

    /**
     * Adjust the order of vertex in every triangle.
     */
    void sort_vertex();

    /**
     *
     */
    void initialize();

    /**
     * Transform the model to view space.
     */
    void view_transform();

    /**
     * Vertex shading.
     */
    void vertex_shading();

    /**
     *
     */
    void clipping();

    /**
     *
     */
    void rasterize();

    /**
     *
     */
    void fragment_shading();

    /**
     *
     */
    void alpha_test();

    /**
     *
     */
    void stencil_test();

    /**
     *
     */
    void depth_test();

    /**
     *
     */
    void dither();

    /**
     * Blend to generate frame.
     */
    void frame_blend();

    /**
     *
     * @param c
     * @param point
     * @param pixel
     */
    inline static void pers_projection(Camera *c, Point3f point, Point2f &pixel);

    /**
     *
     * @param c
     * @param point
     * @param pixel
     */
    inline static void ortho_projection(Camera *c, Point3f point, Point2f &pixel);

    /**
     *
     */
    void shadow_mapping();

    /**
     *
     */
    void shadow(Fragment &frag);

    /**
     *
     * @param p_0
     * @param p_1
     * @param p_2
     * @param t_0
     * @param t_1
     */
    static void get_bounding_box(Point2f p_0, Point2f p_1, Point2f p_2, Point2i &t_0, Point2i &t_1);

    /**
     *
     * @param P_0
     * @param P_1
     * @param P_2
     * @param P
     * @param u
     * @param v
     * @param w
     * @return
     */
    static bool is_in_triangle(const Point2f& P_0, const Point2f& P_1, const Point2f& P_2, const Point2f& P, float &u, float &v, float &w);

    /**
     *
     * @param d_0
     * @param d_1
     * @param d_2
     * @param u
     * @param v
     * @param w
     * @return
     */
    static inline float interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w);
};

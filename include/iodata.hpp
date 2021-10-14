//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_IODATA_HPP
#define RENDERER_IODATA_HPP

#include "State.hpp"
#include "opencv2/highgui.hpp"

#define GAMMA 1/2.2
#define Z_GAMMA 1/2.2

class iodata
{
public:
    /**
     *
     * @param file_name
     */
    static void read_model(const std::string &file_name);

    /**
     * Read ply file data.
     * @param file_name ply file name
     * @return mesh
     */
    static void read_ply(const std::string &file_name, Model *model);

    /**
     * Read DEM data, the vertex number must be over or equal 2x2.
     * @param file_name DEM file name
     * @param model
     * @return mesh
     */
    static void read_DEM(const std::string &file_name, Model *model);

    /**
     * Load config file and initialize the state.
     * @param config
     * @return
     */
    static State *load_config(const std::string &config);

    /**
     *
     * @param mesh
     * @param file_name
     */
    static void write_ply(Mesh *mesh, const std::string &file_name);

    /**
     *
     */
    void clear()
    {
    }

    /**
     * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
     * @param depth_buffer
     * @param c
     */
    static void write_depth_image(const float *depth_buffer, Camera *c);

    /**
     * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
     * @param light
     */
    static void write_depth_image(SunLight *light);

    /**
     *
     * @param frame frame buffer
     */
    static void write_result_image(const FrameBuffer &frame);

    /**
     *
     * @param file_name
     * @param frame
     */
    static void write_result_image(const std::string& file_name, const FrameBuffer &frame);

//    static write_anime_config()
private:
};

#endif

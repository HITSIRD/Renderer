//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_IODATA_HPP
#define RENDERER_IODATA_HPP

#include "State.hpp"

#define GAMMA 1/2.2
#define Z_GAMMA 1/2.2
#define SCALE 1.0f

class iodata
{
public:
    /**
     *
     * @param file_name
     */
    static void readModel(const std::string &file_name);

    /**
     * Read ply file data.
     * @param file_name ply file name
     * @return mesh
     */
    static void readPly(const std::string &file_name, Model *model);

    /**
     * Read DEM data, the vertex number must be over or equal 2x2.
     * @param file_name DEM file name
     * @param model
     * @return mesh
     */
    static void readDEM(const std::string &file_name, Model *model);

    /**
     * Load config file and initialize the state.
     * @param config
     * @return
     */
    static State *loadConfig(const std::string &config);

    /**
     *
     * @param mesh
     * @param file_name
     */
    static void writePlyFile(Mesh *mesh, const std::string &file_name);

    /**
     * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
     * @param depth_buffer
     * @param c
     */
    static void writeDepthImage(const float *depth_buffer, Camera *c);

    /**
     * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
     * @param light
     */
    static void writeDepthImage(SunLight *light);

    /**
     *
     * @param frame frame buffer
     */
    static void writeResultImage(FrameBuffer *frame);

    /**
     *
     * @param file_name
     * @param frame
     */
    static void writeResultImage(const std::string& file_name, FrameBuffer *frame);
};

#endif

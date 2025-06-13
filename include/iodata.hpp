//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_IODATA_HPP
#define RENDERER_IODATA_HPP

#include "State.hpp"
#include "FrameBuffer.hpp"

#define GAMMA 1/2.2
#define Z_GAMMA 1/2.2

class iodata {
public:
    /**
     * Read model file configuration data.
     * @param configPath model configuration file path
     * @param configPath
     * @return model
     */
    static Model *modelConfigParser(const std::string &configPath);

    /**
     * Load material configuration file.
     * @param configPath
     * @return material
     */
    static Material *materialConfigParser(const std::string &configPath);

    /**
     * Load config file and initialize the state.
     * @param configPath
     * @return
     */
    static void renderingConfigParser(const std::string &configPath, State *s);

    /**
     * Read DEM data, the vertex number must be over or equal 2x2.
     * @param fileName DEM file name
     * @return mesh
     */
    static Mesh *readDEM(const std::string &fileName);

    /**
     *
     * @param plyFilePath
     */
    static Mesh *readPlyFile(const std::string &plyFilePath);

    /**
     *
     * @param mesh
     * @param fileName
     */
    static void writePlyFile(Mesh *mesh, const std::string &fileName);

    /**
     * Write result depth image. The gray level manifests the depth, 0(black) means farthest and 255(white) means nearest.
     * @param shadowMap
     */
    static void writeDepthImage(Image<float> *shadowMap);

    /**
     *
     * @param frame frame buffer
     */
    static void writeResultImage(FrameBuffer *frame);

    /**
     *
     * @param fileName
     * @param frame
     */
    static void writeResultImage(const std::string &fileName, FrameBuffer *frame);
};

#endif

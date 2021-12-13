//
// Created by 闻永言 on 2021/8/8.
//

#ifndef RENDERER_RASTERIZER_HPP
#define RENDERER_RASTERIZER_HPP

#include "Type.hpp"
#include "State.hpp"

namespace Render
{
    enum RenderMode{
        DO_LOOP = 0,
        OUTPUT_SINGLE = 1,
        ANIMATION = 2
    };
}

class Rasterizer
{
public:
    State *s;
    bool stop; // if loop should stop rendering

    Rasterizer():stop(false), s(nullptr){}

    Rasterizer(bool _stop):stop(_stop), s(nullptr){}

    ~Rasterizer()
    {
        delete s;
    }

    /**
     *
     * @param modelFile
     * @param configFile
     */
    void readData(const std::string &modelFile, const std::string &configFile);

    /**
     *
     */
    void render(Render::RenderMode renderMode);
private:
    /**
     *
     */
    void renderLoop();

    /**
     *
     */
    void initialize() const;

    /**
     * Homogeneous space clipping.
     * @param v0
     * @param v1
     * @param v2
     * @return
     */
    static bool clipping(const VertexP &v0, const VertexP &v1, const VertexP &v2);

    /**
     * Homogeneous space clipping
     * @param v0
     * @param v1
     * @param v2
     * @return
     */
    static std::vector<VertexP> clip_near(const VertexP &v0, const VertexP &v1, const VertexP &v2);

    /**
     *
     */
    void draw();

    /**
     *
     * @param v0
     * @param v1
     * @param v2
     * @param flatNormal
     */
    void draw_triangle(const VertexP &v0, const VertexP &v1, const VertexP &v2, const float4 &flatNormal);

    /**
     *
     */
    void alphaTest();

    /**
     *
     */
    void stencilTest();

    /**
     * Write fragment color to buffer and test depth.
     * @param _x
     * @param _y
     * @param z fragment z value
     * @param color
     */
    void write_color(int _x, int _y, float z, const float4 &color) const;

    /**
     *
     */
    void dither();

    /**
     * Blend to generate frame.
     */
    void frameBlend();
};

#endif //RENDERER_RASTERIZER_HPP

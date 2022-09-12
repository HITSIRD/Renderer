//
// Created by 闻永言 on 2021/8/8.
//

#ifndef RENDERER_RASTERIZER_HPP
#define RENDERER_RASTERIZER_HPP

#include "Type.hpp"
#include "State.hpp"
#include "FrameBuffer.hpp"

namespace Renderer
{
    enum Pass
    {
        SHADOW_PASS, SCENE_PASS
    };
}

class Rasterizer
{
public:
    State *s;
    FrameBuffer *frameBuffer;
    bool stop; // if loop should stop rendering

    Rasterizer(): stop(false), s(nullptr), frameBuffer(nullptr)
    {}

    Rasterizer(bool _stop): stop(_stop), s(nullptr)
    {}

    ~Rasterizer()
    {
        delete s;
    }

    /**
     *
     * @param modelConfig
     * @param renderingConfig
     */
    void loadConfig(const std::string &modelConfig, const std::string &renderingConfig);

    /**
     *
     * @param renderMode
     */
    void render(Renderer::RenderMode renderMode);

private:
    /**
     *
     */
    void renderSingle();

    /**
     *
     */
    void renderLoop();

    /**
     *
     */
    void renderAnimation();

    /**
     * Initialize buffers and shadow maps.
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
    static std::vector<VertexP> clipNear(const VertexP &v0, const VertexP &v1, const VertexP &v2);

    /**
     *
     */
    void drawShadow();

    /**
     *
     * @param uniform
     * @param pass
     */
    void drawScene(Uniform &uniform, Renderer::Pass pass);

    /**
     *
     * @param v0
     * @param v1
     * @param v2
     */
    void drawTriangle(const VertexP &v0, const VertexP &v1, const VertexP &v2);

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
     * @param _x screen coordinate
     * @param _y screen coordinate
     * @param z fragment z value
     * @param color
     */
    void writeColor(int _x, int _y, float z, const float4 &color) const;

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

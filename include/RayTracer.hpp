//
// Created by 闻永言 on 2021/12/2.
//

#ifndef RENDERER_RAYTRACINGRENDERER_HPP
#define RENDERER_RAYTRACINGRENDERER_HPP

#include "Type.hpp"
#include "State.hpp"
#include "FrameBuffer.hpp"
#include "Ray.hpp"

#define RANDOM_BUFFER_SIZE 1048576 // 1 << 20

namespace Renderer
{
    static const float TMin = -100000.0f; // minimum hit time
    static const float TMax = 100000.0f; // maximum hit time
}

class RayTracer
{
public:
    State *s;
    FrameBuffer *frameBuffer;
    //    bool stop; // if loop should stop rendering

    RayTracer(): s(nullptr)
    {}

    //    Ray(bool _stop):stop(_stop), s(nullptr){}

    ~RayTracer()
    {
        delete s;
    }

    /**
     *
     * @param modelConfig
     * @param renderingConfig
     */
    void readConfig(const std::string &modelConfig, const std::string &renderingConfig);

    /**
     *
     */
    void render(Renderer::RenderMode mode);

private:
    float *randomFloatBuffer; // random float buffer from 0 to 1.0f

    /**
     *
     */
    void renderLoop();

    /**
     *
     */
    void initialize();

    /**
     *
     */
    void draw();

    /**
     *
     * @param x screen coordinate
     * @param y screen coordinate
     * @return
     */
    Ray emitRay(float x, float y) const;

    /**
     *
     */
    void castRay(Ray &ray, int depth) const;

    /**
     *
     * @param ray
     * @param depth
     * @return color
     */
    float4 routeTracing(Ray &ray, int depth) const;

    /**
     * Write fragment color to buffer and test depth.
     * @param x
     * @param y
     * @param color
     */
    void writeColor(int x, int y, const float4 &color) const;
};

#endif //RENDERER_RAYTRACINGRENDERER_HPP

//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_STATE_HPP
#define RENDERER_STATE_HPP

#include "Camera.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Fragment.hpp"
#include "Texture.hpp"

class Shader;

namespace Render
{
    enum RenderScene
    {
        FLAT, PHONG, PBR
    };

    enum FaceCullMode
    {
        NONE, FRONT, BACK
    };

    enum ShadowMode
    {
        SHADOW, NO_SHADOW
    };

    enum TextureType
    {
        NORMAL_TEXTURE, MIPMAP
    };
}

class FrameBuffer
{
public:
    int x;
    int y;
    unsigned char *buffer;

    FrameBuffer()
    {
        x = 0;
        y = 0;
        buffer = nullptr;
    }

    /**
     * Initialize buffer.
     * @param _x
     * @param _y
     */
    FrameBuffer(int _x, int _y);

    ~FrameBuffer();

    /**
     *
     * @param index
     * @param color
     */
    void writeColor(int index, const float4 &color) const;

    /**
     *
     * @param _x
     * @param _y
     * @param color
     */
    void writeColor(int _x, int _y, const float4 &color) const;

    /**
     *
     */
    void reset() const;
};

class State
{
public:
    Model *model; // meshes after clipping, update after each frame
    bool changed; // if current s is changed

    //    RENDER_SCENE render_scene;
    Render::FaceCullMode faceCullMode;
    Render::SamplerType sampler;
    Render::TextureType textureType;
    Render::ShadowMode shadow; // if draw shadow
    size_t maxSample; // maximum sample number

    Camera *camera;
    std::vector<Light *> lightSource; // light source

    Shader *shader;

    //    float *stencil_buffer;
    float *zBuffer; // depth buffer
    //    bool *test_buffer; // test
    float4 *colorBuffer; // store current pixel color
    FrameBuffer *frameBuffer;

    int numThreads; // max thread number

    State():model(new Model()), changed(true), faceCullMode(Render::BACK), sampler(Render::NORMAL),
            textureType(Render::NORMAL_TEXTURE), shadow(Render::NO_SHADOW), maxSample(16), camera(nullptr),
            shader(nullptr), zBuffer(nullptr),
            //    test_buffer(nullptr),
            colorBuffer(nullptr), numThreads(0){}

    ~State();

    /**
     *
     */
    void resetBuffer() const;

    /**
     *
     */
    void clear();

    /**
     *
     */
    void check();

    /**
     *
     */
    void destroy() const;
};

#endif //RENDERER_STATE_HPP

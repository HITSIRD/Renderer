//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_STATE_HPP
#define RENDERER_STATE_HPP

#include "Light.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

namespace Renderer {
    enum RenderMode {
        DO_LOOP = 0, OUTPUT_SINGLE = 1, ANIMATION = 2
    };

    enum RenderScene {
        FLAT, PHONG, PBR
    };

    enum FaceCullMode {
        NONE, FRONT, BACK
    };

    enum ShadowMode {
        SHADOW, NO_SHADOW
    };

    enum Projection {
        PERS, ORTHO
    };
}

class State {
public:
    int2 screenSize;

    Model *model; // meshes after clipping, update after each frame
    bool changed; // if current s is changed

    Renderer::Projection projection;
    Renderer::FaceCullMode faceCullMode;
    Renderer::SamplerType sampler;
    bool mipmap;
    Renderer::ShadowMode shadow; // if draw shadow
    bool depthTest;
    size_t maxSample; // maximum sample number

    Camera *camera;
    std::vector<Light *> lightSource; // light source

    Shader *shader;

    Image<float> *zBuffer;
    //    float *stencil_buffer;
    //    bool *test_buffer; // test

    int numThreads; // max thread number

    State();

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

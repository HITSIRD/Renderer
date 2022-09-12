//
// Created by 闻永言 on 2021/12/23.
//

#ifndef RENDERER_SUNLIGHTSHADOWSHADER_HPP
#define RENDERER_SUNLIGHTSHADOWSHADER_HPP

#include "ShadowShader.hpp"

class SunLightShadowShader: public ShadowShader
{
    static SunLightShadowShader *shader;

    SunLightShadowShader() = default;

public:
    ~SunLightShadowShader() override;

    static SunLightShadowShader *instance();

    void vertexShader(VertexP &vertex) override;

    void destroy() override;
};

#endif //RENDERER_SUNLIGHTSHADOWSHADER_HPP

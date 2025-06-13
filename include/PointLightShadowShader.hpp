//
// Created by 闻永言 on 2021/12/23.
//

#ifndef RENDERER_POINTLIGHTSHADOWSHADER_HPP
#define RENDERER_POINTLIGHTSHADOWSHADER_HPP

#include "ShadowShader.hpp"

class PointLightShadowShader : public ShadowShader {
    static PointLightShadowShader *shader;

    PointLightShadowShader() = default;

public:
    ~PointLightShadowShader() override;

    static PointLightShadowShader *instance();

    void vertexShader(VertexP &vertex) override;

    void destroy() override;
};

#endif //RENDERER_POINTLIGHTSHADOWSHADER_HPP

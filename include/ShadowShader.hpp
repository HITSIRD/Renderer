//
// Created by 闻永言 on 2021/12/23.
//

#ifndef RENDERER_SHADOWSHADER_HPP
#define RENDERER_SHADOWSHADER_HPP

#include "Shader.hpp"

class ShadowShader: public Shader
{
    static ShadowShader *shader;

public:
    ShadowShader() = default;

    virtual ~ShadowShader() = default;

    static ShadowShader *instance()
    {
        if (!shader)
        {
            shader = new ShadowShader();
        }
        return shader;
    }

    virtual void vertexShader(VertexP &vertex)
    {}

    virtual void destroy()
    {}
};

#endif //RENDERER_SHADOWSHADER_HPP

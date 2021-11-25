//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_FLATSHADER_HPP
#define RENDERER_FLATSHADER_HPP

#include "Shader.hpp"

class FlatShader:public Shader
{
    static FlatShader *shader;

    FlatShader() = default;
public:
    ~FlatShader() override;

    /**
     * Get a instance and reset.
     */
    static FlatShader *instance();

    /**
     * Shader. Automatically select the type.
     * @param vertex
     */
    void vertex_shader(VertexP &vertex) override;

    /**
     * Shader. Automatically select the type.
     * @param frag
     */
    void fragment_shader(Fragment &frag) override;

    void destroy() override;

    static float shadow(Light *light, const Fragment &frag);
};

#endif //RENDERER_FLATSHADER_HPP

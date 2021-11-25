//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_PHONGSHADER_HPP
#define RENDERER_PHONGSHADER_HPP

#include "Shader.hpp"

class PhongShader: public Shader
{
    static PhongShader *shader;

    PhongShader()= default;
public:
    ~PhongShader() override;

    /**
     * Get a instance and reset.
     */
    static PhongShader *instance();

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

    void destroy() override
    {
        delete uniform;
        uniform = nullptr;
    }

    static float shadow(Light *light, const Fragment &frag);
};

#endif //RENDERER_PHONGSHADER_HPP

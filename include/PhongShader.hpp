//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_PHONGSHADER_HPP
#define RENDERER_PHONGSHADER_HPP

#include "Shader.hpp"

class PhongShader : public Shader {
    static PhongShader *shader;

    PhongShader() = default;

public:
    ~PhongShader() override;

    /**
     * Get a getInstance and reset.
     */
    static PhongShader *instance();

    /**
     * Vertex Shader. Automatically select the type.
     * @param vertex
     */
    void vertexShader(VertexP &vertex) override;

    /**
     * Fragment shader. Automatically select the type.
     * @param frag
     * @return color
     */
    float4 fragmentShader(Fragment &frag) override;

    /**
     *
     * @param record
     * @return color
     */
    float4 rayShader(HitRecord &record) override;

    /**
     *
     */
    void destroy() override {
        // delete uniform;
        // uniform = nullptr;
    }

    /**
     *
     * @param light
     * @param frag
     * @return
     */
    static float shadow(Light *light, const Fragment &frag);
};

#endif //RENDERER_PHONGSHADER_HPP

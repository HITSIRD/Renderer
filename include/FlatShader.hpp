//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_FLATSHADER_HPP
#define RENDERER_FLATSHADER_HPP

#include "Shader.hpp"

class FlatShader : public Shader {
    static FlatShader *shader;

    FlatShader() = default;

public:
    ~FlatShader() override;

    /**
     * Get a getInstance and reset.
     */
    static FlatShader *instance();

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
    void destroy() override;

    /**
     *
     * @param light
     * @param frag
     * @return
     */
    static float shadow(Light *light, const Fragment &frag);
};

#endif //RENDERER_FLATSHADER_HPP

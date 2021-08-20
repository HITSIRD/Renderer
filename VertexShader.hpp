//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_VERTEXSHADER_HPP
#define RENDERER_VERTEXSHADER_HPP

#include "Shader.hpp"
#include <utility>

class VertexShader:public Shader
{
public:
    VertexShader(float in_ka, float in_kd, float in_ks, float in_spec, Vec3 in_light)
    {
        ka = in_ka;
        kd = in_kd;
        ks = in_ks;
        light = std::move(in_light);
        view << 0, 0, -1, 0;
    }

    /**
     * Shader. Automatically select the type.
     * @param vertex
     * @param shading_type
     */
    void shading(Vertex &vertex, int shading_type);

    /**
     *
     * @param vertex
     */
    void flat_shading(Vertex &vertex);

    /**
     *
     * @param vertex
     */
    void phong_shading(Vertex &vertex);
};

#endif //RENDERER_VERTEXSHADER_HPP

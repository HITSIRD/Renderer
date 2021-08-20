//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_FRAGMENTSHADER_HPP
#define RENDERER_FRAGMENTSHADER_HPP

#include "Shader.hpp"
#include "Mesh.hpp"
#include <utility>
#include "Camera.hpp"

class FragmentShader:public Shader
{
public:
    FragmentShader(float in_ka, float in_kd, float in_ks, float in_spec, Vec3 in_light)
    {
        ka = in_ka;
        kd = in_kd;
        ks = in_ks;
        light = std::move(in_light);
        view << 0, 0, -1, 0;
    }

    /**
     * Shader. Automatically select the type.
     * @param fragment
     * @param shading_type
     */
    void shading(Fragment &fragment, int shading_type);

    /**
     *
     * @param fragment
     */
    void flat_shading(Fragment &fragment);

    /**
     *
     * @param fragment
     */
    void phong_shading(Fragment &fragment);
};

#endif //RENDERER_FRAGMENTSHADER_HPP

//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_SHADER_HPP
#define RENDERER_SHADER_HPP

#include "Vertex.hpp"
#include "Fragment.hpp"

#define FLAT_SHADING 1
#define PHONG_SHADING 2
#define EPSILON 0.5f

class Shader
{
protected:
    float ka; // ambient coefficient
    float kd; // diffuse coefficient
    float ks; // specular coefficient

    float spec_rank; // power of specular coefficient

    Vec3 light;
    Vec3 view; // camera view vector

public:
//    virtual void shading(Vertex &vertex, int shading_type);
};

#endif

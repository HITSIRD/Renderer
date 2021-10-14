//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_VERTEX_HPP
#define RENDERER_VERTEX_HPP

#include "Type.hpp"
#include <utility>

class Vertex
{
public:
    float4 world; // coordinate in world space
    float4 clip; // coordinate in is_clip space
    float4 screen; // screen space coordinate
    float4 color; // R, G, B, A
    float4 normal; // normal vector
    float2 texture_uv; // base_texture coordinate
    float z;

    /**
     *
     * @param v world space coordinate
     */
    Vertex(float4 v):world(std::move(v)), color(1.0f, 1.0f, 1.0f, 1.0f), texture_uv(0, 0){}

    /**
     *
     * @param v world space coordinate
     * @param c vertex color
     */
    Vertex(float4 v, float4 c):world(std::move(v)), color(std::move(c)), texture_uv(0, 0){}

    /**
     *
     * @param v
     * @param _uv base_texture coordination
     */
    Vertex(float4 v, float2 _uv):world(std::move(v)), color(1.0f, 1.0f, 1.0f, 1.0f), texture_uv(std::move(_uv)){};

    Vertex(float4 _world, float4 _clip, float4 _color, float4 _normal, float2 _texture_uv):
        world(std::move(_world)),
        clip(std::move(_clip)),
        color(std::move(_color)),
        normal(std::move(_normal)),
        texture_uv(std::move(_texture_uv)){}
};

#endif //RENDERER_VERTEX_HPP

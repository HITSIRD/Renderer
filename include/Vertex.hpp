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
    float4 position; // coordinate in world space
//    float4 clip; // coordinate in is_clip space
//    float4 screen; // screen space coordinate
    float4 color; // R, G, B, A
    float4 normal; // normal vector
    float2 texture_uv; // base_texture coordinate
//    float z;

    Vertex(){}

    /**
     *
     * @param v world space coordinate
     */
    Vertex(float4 v):position(std::move(v)), color(1.0f, 1.0f, 1.0f, 1.0f), texture_uv(0, 0){}

    /**
     *
     * @param v world space coordinate
     * @param c vertex color
     */
    Vertex(float4 v, float4 c):position(std::move(v)), color(std::move(c)), texture_uv(0, 0){}

    /**
     *
     * @param v
     * @param _uv base_texture coordination
     */
    Vertex(float4 v, float2 _uv):position(std::move(v)), color(1.0f, 1.0f, 1.0f, 1.0f), texture_uv(std::move(_uv)){};

    Vertex(float4 _world, float4 _color, float4 _normal, float2 _texture_uv):
            position(std::move(_world)),
//            clip(std::move(_clip)),
            color(std::move(_color)),
            normal(std::move(_normal)),
            texture_uv(std::move(_texture_uv)){}
};

/**
 * Vertex to process in pipeline.
 */
class VertexP
{
public:
    float4 position; // coordinate in world space
    float4 clip; // coordinate in is_clip space
    float4 screen; // screen space coordinate
    float4 color; // R, G, B, A
    float4 normal; // normal vector
    float2 texture_uv; // base_texture coordinate
    float z_rec; // reciprocal of clip.w, equals 1 / camera_position.z

    VertexP(){}

    /**
     *
     * @param v vertex
     */
    VertexP(const Vertex &v):position(v.position), color(v.color), normal(v.normal), texture_uv(v.texture_uv){}
};

#endif //RENDERER_VERTEX_HPP

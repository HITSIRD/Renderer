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
    float4 color; // R, G, B, A
    float4 normal; // normal vector
    float4 tangent; // tangent vector
    float2 textureCoord; // base_texture coordinate

    Vertex()
    {}

    /**
     *
     * @param v world space coordinate
     */
    Vertex(float4 v): position(std::move(v)), color(1.0f, 1.0f, 1.0f, 1.0f), textureCoord(0, 0)
    {}

    /**
     *
     * @param v world space coordinate
     * @param c vertex color
     */
    Vertex(float4 v, float4 c): position(std::move(v)), color(std::move(c)), textureCoord(0, 0)
    {}

    /**
     *
     * @param v
     * @param _uv base_texture coordination
     */
    Vertex(float4 v, float2 _uv): position(std::move(v)), color(1.0f, 1.0f, 1.0f, 1.0f), textureCoord(std::move(_uv))
    {};

    Vertex(float4 _world, float4 _color, float4 _normal, float4 _tangent, float2 _texture_uv): position(
            std::move(_world)), color(std::move(_color)), normal(std::move(_normal)), tangent(std::move(_tangent)),
            textureCoord(std::move(_texture_uv))
    {}
};

/**
 * Vertex to process in pipeline.
 */
class VertexP
{
public:
    float4 position; // coordinate in world space
    float4 screen; // screen space coordinate, can be clip space in specific stage
    float4 color; // R, G, B, A
    float4 normal; // normal vector
    float4 tangent; // tangent vector
    float2 textureCoord; // base_texture coordinate
    float zRec; // reciprocal of clip.w, equals 1 / camera_position.z

    VertexP()
    {}

    /**
     *
     * @param v vertex
     */
    VertexP(const Vertex &v): position(v.position), color(v.color), normal(v.normal), tangent(v.tangent),
            textureCoord(v.textureCoord)
    {}
};

#endif //RENDERER_VERTEX_HPP

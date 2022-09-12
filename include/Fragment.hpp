//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_FRAGMENT_HPP
#define RENDERER_FRAGMENT_HPP

#include "Type.hpp"

class Fragment
{
public:
    float4 color; // R, G, B, A
    float4 flatNormal; // flat normal vector
    float4 normal; // interpolation normal vector
    float4 world; // world space
    float4 tangent; // tangent vector
    float2 textureCoord; // texture coordinate
    float clipZ;

    float2 ddx; // to get du/dx, dv/dx
    float2 ddy; // to get du/dy, dv/dy

    Fragment()
    {}

    ~Fragment() = default;
};

#endif //RENDERER_FRAGMENT_HPP

//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_FRAGMENT_HPP
#define RENDERER_FRAGMENT_HPP

#include "Type.hpp"

class Fragment
{
public:
    int x, y; // index of fragment on screen
    float4 color; // R, G, B, A
    float4 flat_normal; // flat normal vector
    float4 normal; // interpolation normal vector
    float4 world; // world space
//    vec4 screen; // screen space coordinate
    float2 texture_uv;
    float clip_z;
    float z;

    Fragment(){}

    /**
     *
     * @param in_x
     * @param in_y
     */
    Fragment(int _x, int _y):x(_x), y(_y){}

    ~Fragment() = default;
};

#endif //RENDERER_FRAGMENT_HPP

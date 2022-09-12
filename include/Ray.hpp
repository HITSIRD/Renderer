#include <utility>

//
// Created by 闻永言 on 2021/12/3.
//

#ifndef RENDERER_RAY_HPP
#define RENDERER_RAY_HPP

namespace Renderer
{
    static const float Epsilon = 0.0005f;
}

class Ray
{
public:
    float4 origin; // start position in world space
    float4 direction; // direction in world space

//    float intensity;

    Ray() {}

    Ray(float4 _origin, float4 _direction): origin(std::move(_origin)), direction(std::move(_direction)) {}

    ~Ray() = default;
};

#endif //RENDERER_RAY_HPP

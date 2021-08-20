//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_VERTEX_HPP
#define RENDERER_VERTEX_HPP

#include "Type.hpp"
#include <utility>

class Vertex{
public:
    Vec3 world; // coordinate in world space
    Vec3 view; // coordinate in view space
    float color; // gray color, [0, 1]
    Vec3 normal; // normal vector
    Eigen::Vector2f uv; // texture coordinate

    /**
     *
     * @param v world space coordinate
     */
    Vertex(Vec3 v)
    {
        world = std::move(v);
        color = 0;
    }
};

#endif //RENDERER_VERTEX_HPP


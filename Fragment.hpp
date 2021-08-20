//
// Created by 闻永言 on 2021/8/15.
//

#ifndef RENDERER_FRAGMENT_HPP
#define RENDERER_FRAGMENT_HPP

#include "Type.hpp"

class Fragment
{
public:
    uint16_t x, y; // index of fragment on screen
    float z; // depth of current fragment
    float color; // gray color, [0, 1]
    Vec3 normal; // normal vector
    Point3f point;

    /**
     *
     * @param in_x
     * @param in_y
     * @param z max depth
     */
    Fragment(uint16_t in_x, uint16_t in_y, float z_buffer)
    {
        x = in_x;
        y = in_y;
        z = z_buffer;
        color = 0;
    }
};

#endif //RENDERER_FRAGMENT_HPP

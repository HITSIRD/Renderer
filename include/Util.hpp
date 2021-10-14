//
// Created by 闻永言 on 2021/9/17.
//

#ifndef RENDERER_UTIL_HPP
#define RENDERER_UTIL_HPP

#include "Fragment.hpp"
#include "Vertex.hpp"

#define PI 3.1415926536f

/**
 *
 * @param deg
 * @return
 */
inline float deg2rad(float deg)
{
    return deg * PI / 180.0f;
}

/**
 *
 * @param rad
 * @return
 */
inline float rad2deg(float rad)
{
    return rad * 180.0f / PI;
}

/**
 *
 * @tparam T
 * @param v0
 * @param v1
 * @param v2
 * @param u
 * @param v
 * @param w
 * @return
 */
template<typename T> inline T lerp(const T &v0, const T &v1, const T &v2, float u, float v, float w)
{
    return u * v0 + v * v1 + w * v2;
}

/**
 *
 * @param v_0
 * @param v_1
 * @param v_2
 * @param u
 * @param v
 * @param w
 * @return
 */
inline Fragment lerp(const Vertex &v_0, const Vertex &v_1, const Vertex &v_2, float u, float v, float w)
{
    Fragment frag;
    frag.world = lerp(v_0.world, v_1.world, v_2.world, u, v, w);
    frag.normal = lerp(v_0.normal, v_1.normal, v_2.normal, u, v, w);
    frag.color = lerp(v_0.color, v_1.color, v_2.color, u, v, w);
    frag.texture_uv = lerp(v_0.texture_uv, v_1.texture_uv, v_2.texture_uv, u, v, w);
    frag.clip_z = 1.0f / lerp(v_0.z, v_1.z, v_2.z, u, v, w);
    return frag;
}

/**
 * Judge if a point is in a triangle.
 * @param AB
 * @param BC
 * @param CA
 * @param u
 * @param v
 * @param w
 * @return
 */
inline bool is_in_triangle(float AB, float BC, float CA, float &u, float &v, float &w)
{
    float S = 1.0f / (AB + BC + CA);

//    u = BC * S;
//    if (u < 0 || u > 1.0f)
//    {
//        return false;
//    }
//
//    v = CA * S;
//    if (v < 0 || v > 1.0f)
//    {
//        return false;
//    }
//
//    w = AB * S;
//    if (w < 0 || w > 1.0f)
//    {
//        return false;
//    }
    u = BC * S;
    v = CA * S;
    w = AB * S;
    return u < 1.0f && v < 1.0f && w < 1.0f;
}

/**
 *
 * @param vertex
 */
inline void perspective_division(Vertex &vertex)
{
    vertex.world *= vertex.z;
    vertex.normal *= vertex.z;
    vertex.color *= vertex.z;
    vertex.texture_uv *= vertex.z;
}

/**
 *
 * @param frag
 */
inline void perspective_restore(Fragment &frag)
{
    frag.world *= frag.clip_z;
    frag.normal *= frag.clip_z;
    frag.color *= frag.clip_z;
    frag.texture_uv *= frag.clip_z;
}

/**
 *
 * @param vertex
 */
inline void perspective_restore(Vertex &vertex)
{
    vertex.z = 1.0f / vertex.z;
    vertex.world *= vertex.z;
    vertex.normal *= vertex.z;
    vertex.color *= vertex.z;
    vertex.texture_uv *= vertex.z;
}

#endif //RENDERER_UTIL_HPP

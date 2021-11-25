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
 * @param x
 * @param v_0
 * @param v_1
 * @return
 */
template<typename T> inline T lerp(float x, T v_0, T v_1)
{
    return v_0 + x * (v_1 - v_0);
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
inline Fragment lerp(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2, float u, float v, float w)
{
    Fragment frag;
    frag.world = lerp(v_0.position, v_1.position, v_2.position, u, v, w);
    frag.normal = lerp(v_0.normal, v_1.normal, v_2.normal, u, v, w);
    frag.color = lerp(v_0.color, v_1.color, v_2.color, u, v, w);
    frag.texture_uv = lerp(v_0.texture_uv, v_1.texture_uv, v_2.texture_uv, u, v, w);
    frag.clip_z = 1.0f / lerp(v_0.z_rec, v_1.z_rec, v_2.z_rec, u, v, w);
    return frag;
}

/**
 *
 * @param x
 * @param v_0
 * @param v_1
 * @return
 */
inline VertexP lerp(float x, const VertexP &v_0, const VertexP &v_1)
{
    VertexP v;
    v.position = lerp(x, v_0.position, v_1.position);
    v.clip = lerp(x, v_0.clip, v_1.clip);
    v.color = lerp(x, v_0.color, v_1.color);
    v.texture_uv = lerp(x, v_0.texture_uv, v_1.texture_uv);
    v.z_rec = 1.0f / v.clip.w();
    return v;
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
inline void perspective_division(VertexP &vertex)
{
    vertex.position *= vertex.z_rec;
    vertex.normal *= vertex.z_rec;
    vertex.color *= vertex.z_rec;
    vertex.texture_uv *= vertex.z_rec;
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
    frag.texture_x *= frag.clip_z;
    frag.texture_y *= frag.clip_z;
}

/**
 *
 * @param vertex
 */
inline void perspective_restore(VertexP &vertex)
{
    float z = 1.0f / vertex.z_rec;
    vertex.position *= z;
    vertex.normal *= z;
    vertex.color *= z;
    vertex.texture_uv *= z;
}

#endif //RENDERER_UTIL_HPP

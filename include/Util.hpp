//
// Created by 闻永言 on 2021/9/17.
//

#ifndef RENDERER_UTIL_HPP
#define RENDERER_UTIL_HPP

#include "Fragment.hpp"
#include "Vertex.hpp"
#include "random"

namespace Renderer
{
    /**
     * utility constants
     */
    static const float Pi = 3.1415926535897932f;
    static const float Pi2 = 6.2831853071795865f; // 2 * Pi
    static const float InvPi = 0.3183098861837907f; // 1 / Pi
    static const float Inv2Pi = 0.6366197723675813f; // 2 / Pi
    static const float Inv4Pi = 1.2732395447351627f; // 4 / Pi
    static const float PiOver2 = 1.5707963267948966f; // Pi / 2
    static const float PiOver4 = 0.7853981633974483f; // Pi / 4
    static const float Sqrt2 = 1.4142135623730950f; // Pi / 4
    static const float Log2 = 0.6931471805599453f; // log(2)
    static const float InvLog2 = 1.4426950408889634f; // 1 / log(2)
    static const float Inv255 = 0.0039215686274510f; // 1 / 255.0
    static const float Inv65535 = 0.0000152590218967f; // 1 / 65535.0

    static const float4 ZeroFloat4(0, 0, 0, 0);
    static const float4
            RandomFloat4(0.5305025836169190f, 0.4564595701410036f, -0.7142910258448331f, 0); // random float4 vector

    /**
     *
     * @param deg
     * @return
     */
    inline float deg2rad(float deg)
    {
        return deg * Pi / 180.0f;
    }

    /**
     *
     * @param rad
     * @return
     */
    inline float rad2deg(float rad)
    {
        return rad * 180.0f / Pi;
    }

    /**
    * Calculate rotation matrix of H, P, B.
    * @param H
    * @param P
    * @param B
    */
    void calculateHPB(float3x3 &H, float3x3 &P, float3x3 &B);

    /**
     * Calculate rotation matrix l_R3.
     */
    float3x3 calculateRotateMatrix();

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
     * @param v0
     * @param v1
     * @param v2
     * @param u
     * @param v
     * @param w
     * @return
     */
    inline Fragment lerp(const VertexP &v0, const VertexP &v1, const VertexP &v2, float u, float v, float w)
    {
        Fragment frag;
        frag.world = lerp(v0.position, v1.position, v2.position, u, v, w);
        frag.normal = lerp(v0.normal, v1.normal, v2.normal, u, v, w);
        frag.color = lerp(v0.color, v1.color, v2.color, u, v, w);
        frag.textureCoord = lerp(v0.textureCoord, v1.textureCoord, v2.textureCoord, u, v, w);
        frag.tangent = lerp(v0.tangent, v1.tangent, v2.tangent, u, v, w);
        frag.clipZ = 1.0f / lerp(v0.zRec, v1.zRec, v2.zRec, u, v, w);
        return frag;
    }

    /**
     *
     * @param x
     * @param v0
     * @param v1
     * @return
     */
    inline VertexP lerp(float x, const VertexP &v0, const VertexP &v1)
    {
        VertexP v;
        v.position = lerp(x, v0.position, v1.position);
        v.screen = lerp(x, v0.screen, v1.screen);
        v.color = lerp(x, v0.color, v1.color);
        v.tangent = lerp(x, v0.tangent, v1.tangent);
        v.textureCoord = lerp(x, v0.textureCoord, v1.textureCoord);
        v.zRec = 1.0f / v.screen.w();
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
    inline bool isInTriangle(float AB, float BC, float CA, float &u, float &v, float &w)
    {
        float S = 1.0f / (AB + BC + CA);

        u = BC * S;
        if (u < 0 || u > 1.0f)
        {
            return false;
        }

        v = CA * S;
        if (v < 0 || v > 1.0f)
        {
            return false;
        }

        w = AB * S;
        if (w < 0 || w > 1.0f)
        {
            return false;
        }
        u = BC * S;
        v = CA * S;
        w = AB * S;
        return u < 1.0f && v < 1.0f && w < 1.0f;
    }

    /**
     *
     * @param vertex
     */
    inline void perspectiveDivision(VertexP &vertex)
    {
        vertex.position *= vertex.zRec;
        vertex.normal *= vertex.zRec;
        vertex.color *= vertex.zRec;
        vertex.textureCoord *= vertex.zRec;
    }

    /**
     *
     * @param frag
     */
    inline void perspectiveRestore(Fragment &frag)
    {
        frag.world *= frag.clipZ;
        frag.normal *= frag.clipZ;
        frag.color *= frag.clipZ;
        frag.textureCoord *= frag.clipZ;
        frag.ddx *= frag.clipZ;
        frag.ddy *= frag.clipZ;
    }

    /**
     *
     * @param vertex
     */
    inline void perspectiveRestore(VertexP &vertex)
    {
        float z = 1.0f / vertex.zRec;
        vertex.position *= z;
        vertex.normal *= z;
        vertex.color *= z;
        vertex.textureCoord *= z;
    }

    /**
     *
     * @param v0
     * @param v1
     * @param v2
     * @return tangent of vertices in a triangle
     */
    inline float4 calculateTangent(const Vertex &v0, const Vertex &v1, const Vertex &v2)
    {
        float4 e1 = v1.position - v0.position;
        float4 e2 = v2.position - v0.position;
        float2 dUV1 = v1.textureCoord - v0.textureCoord;
        float2 dUV2 = v2.textureCoord - v0.textureCoord;
        float determinant = dUV1.x() * dUV2.y() - dUV2.x() * dUV1.y();
        if (determinant == 0) // singular matrix
        {
            return RandomFloat4;
        }
        float invDet = 1.f / (dUV1.x() * dUV2.y() - dUV2.x() * dUV1.y());
        return {invDet * (dUV2.y() * e1.x() - dUV1.y() * e2.x()), invDet * (dUV2.y() * e1.y() - dUV1.y() * e2.y()),
                invDet * (dUV2.y() * e1.z() - dUV1.y() * e2.z()), 0};
    }

    /**
     *
     * @param t00
     * @param t01
     * @param t10
     * @param t11
     * @return
     */
    inline bool overlap(float t00, float t01, float t10, float t11)
    {
        return std::max(t00, t10) < std::min(t01, t11);
    }

    /**
     * Get a random number obeys the Gauss distribution.
     * @param mu
     * @param sigma
     * @return
     */
    static float gaussianRand(float mu, float sigma)
    {
        std::default_random_engine e;
        std::random_device rd;
        std::normal_distribution<> gaussian(mu, sigma);
        e.seed(rd());
        return gaussian(e);
    }

    /**
     * Get a uniform random between min and max.
     * @param min
     * @param max
     * @return
     */
    static float uniformRandom(float min, float max)
    {
        std::default_random_engine e;
        std::random_device rd;
        std::uniform_real_distribution<> uniform(min, max);
        e.seed(rd());
        return uniform(e);
    }

    /**
     *
     * @param size
     * @return
     */
    static float *getRandomFloatBuffer(int size)
    {
        std::default_random_engine e;
        std::random_device rd;
        std::uniform_real_distribution<> uniform(0, 1.0f);
        e.seed(rd());
        float *buffer = new float[size];
        for (int i = 0; i < size; i++)
        {
            buffer[i] = uniform(e);
        }

        return buffer;
    }

    /**
     * Get a uniform random vector between min and max.
     * @param min
     * @param max
     * @return
     */
    static float4 uniformRandomVector(float min, float max)
    {
        std::default_random_engine e;
        std::random_device rd;
        std::uniform_real_distribution<> uniform(min, max);
        e.seed(rd());
        return {uniform(e), uniform(e), uniform(e), 0};
    }
}

#endif //RENDERER_UTIL_HPP

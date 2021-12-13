//
// Created by 闻永言 on 2021/8/28.
//

#include "Light.hpp"
#include "Util.hpp"
#include "Eigen/Geometry"

using namespace std;
using namespace Render;

Light::~Light() = default;

void Light::shadowMapping(Model *model){}

PointLight::PointLight(float power, int _shadowSize, float4 _positon)
{
    type = POINT;
    intensity = power;
    shadowSize = _shadowSize;

    position = std::move(_positon); // left system to right system
    shadowMap = nullptr;
}

void PointLight::shadowMapping(Model *model)
{
    //    delete shadow_map;
    //    shadow_map = new ShadowMap(x, y);
    //    for (int i = 0; i < x * y; i++)
    //    {
    //        shadow_map->map[i] = 1.0f;
    //    }
    //
    //    for (auto mesh: model.meshes)
    //    {
    //        auto *point_2d = new vec4[mesh.num_vertex];
    //        // Calculate depth, back projection point_2d coordinate of every vertex
    //        for (int i = 0; i < mesh.num_vertex; i++)
    //        {
    //            vec4 clip = P * mesh.vertices[i].world;
    //            point_2d[i] = clip / clip.w();
    //        }
    //
    //        //#pragma omp parallel for
    //        for (const auto &triangle:mesh.triangles)
    //        {
    //            vec4 P_0 = point_2d[triangle.vertex_0];
    //            vec4 P_1 = point_2d[triangle.vertex_1];
    //            vec4 P_2 = point_2d[triangle.vertex_2];
    //
    //            vec4i T_0, T_1;
    //            get_bounding_box(P_0, P_1, P_2, T_0, T_1);
    //
    //            float u, v, w;
    //
    //            // real bounding box cut, if is after clipping, it can be removed
    //            if (T_0.x() >= x || T_0.y() >= y || T_1.x() < 0 || T_1.y() < 0)
    //            {
    //                continue;
    //            }
    //            T_0.x() = T_0.x() < 0 ? 0 : T_0.x();
    //            T_0.y() = T_0.y() < 0 ? 0 : T_0.y();
    //            T_1.x() = T_1.x() >= x ? x - 1 : T_1.x();
    //            T_1.y() = T_1.y() >= y ? y - 1 : T_1.y();
    //
    //            for (int i = T_0.y(); i <= T_1.y(); i++)
    //            {
    //                for (int j = T_0.x(); j <= T_1.x(); j++)
    //                {
    //                    vec4 pixel_center(float(j) + 0.5f, float(i) + 0.5f, 1.0f, 1.0f);
    //                    if (is_in_triangle(P_0, P_1, P_2, pixel_center, u, v, w))
    //                    {
    //                        float depth = interpolate_depth(P_0.z(), P_1.z(), P_2.z(), u, v, w);
    //                        if (depth < shadow_map->map[i * x + j])
    //                        {
    //                            shadow_map->map[i * x + j] = depth;
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //        delete[] point_2d;
    //    }
}

void SunLight::setIntensity(float _intensity)
{
    intensity = _intensity;
}

void SunLight::setViewport(int _x, int _y, float ccdSizeX, float ccdSizeY, float focalLength)
{
    x = _x;
    y = _y;
    float FovH = 2 * atanf(ccdSizeX / (2 * focalLength));
    float FovV = 2 * atanf(ccdSizeY / (2 * focalLength));

    n = focalLength / 1000.0f; // mm to m
    f = 10000.0f; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    matrixOrthographic << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
              0, 2.0f / (t - b), 0, (b + t) / (b - t),
              0, 0, 1.0f / (f - n),
              n / (n - f), 0, 0, 0, 1.0f;

    matrixViewport << (float)x * 0.5f, 0, 0, (float)x * 0.5f,
              0, (float)y * 0.5f, 0, (float)y *0.5f,
              0, 0, 1.0f, 0,
              0, 0, 0, 1.0f;
}

void SunLight::setViewport(int _x, int _y, float range)
{
    x = _x;
    y = _y;

    n = 0.1f; // near clip plane
    f = 10000.0f; // far clip plane
    float r = range * 0.5f;
    float l = -r;
    float t = range * 0.5f * (float)y / (float)x;
    float b = -t;

    matrixOrthographic << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
              0, 2.0f / (t - b), 0, (b + t) / (b - t),
              0, 0, 1.0f / (f - n),
              n / (n - f), 0, 0, 0, 1.0f;

    matrixViewport << (float)x * 0.5f, 0, 0, (float)x * 0.5f,
              0, (float)y * 0.5f, 0, (float)y * 0.5f,
              0, 0, 1.0f, 0,
              0, 0, 0, 1.0f;
}

void SunLight::setLookAt(float4 _position, float4 _focal, float4 _up)
{
    position = std::move(_position);
    up = std::move(_up);
    float4 Z = (_focal - position).normalized();
    direct = -Z;
    float4 X = Z.cross3(up).normalized();
    float4 Y = Z.cross3(X).normalized();
    float4 temp(0, 0, 0, 0);
    matrixView << X, Y, Z, temp;
    matrixView.transposeInPlace();
    float4 translation(-X.dot(position), -Y.dot(position), -Z.dot(position), 1.0f);
    matrixView.col(3) = translation;

    matrixWorldToScreen = matrixViewport * matrixOrthographic * matrixView;
}

void SunLight::shadowMapping(Model *model)
{
    if (shadowMap)
    {
        shadowMap->reset();
    } else
    {
        shadowMap = new ShadowMap(x, y);
    }

    for (const auto &mesh: model->meshes)
    {
        auto *point_2d = new float4[mesh->numVertices];
        // Calculate depth, back projection point_2d coordinate of every vertex
        for (int i = 0; i < mesh->numVertices; i++)
        {
            point_2d[i] = matrixWorldToScreen * mesh->vertices[i].position;
        }

#pragma omp parallel for
        for (const auto &triangle: mesh->triangles)
        {
            float4 v_0 = point_2d[triangle.vertexIndex[0]];
            float4 v_1 = point_2d[triangle.vertexIndex[1]];
            float4 v_2 = point_2d[triangle.vertexIndex[2]];

            // real bounding box
            int min_x = max((int)min(v_0.x(), min(v_1.x(), v_2.x())), 0);
            int min_y = max((int)min(v_0.y(), min(v_1.y(), v_2.y())), 0);
            int max_x = min((int)max(v_0.x(), max(v_1.x(), v_2.x())) + 1, x - 1);
            int max_y = min((int)max(v_0.y(), max(v_1.y(), v_2.y())) + 1, y - 1);

            // rasterization
            for (int i = min_y; i <= max_y; i++)
            {
                int index = i * x;
                for (int j = min_x; j <= max_x; j++)
                {
                    float center_x = float(j) + 0.5f;
                    float center_y = float(i) + 0.5f;
                    float v0x = v_0.x() - center_x;
                    float v0y = v_0.y() - center_y;
                    float v1x = v_1.x() - center_x;
                    float v1y = v_1.y() - center_y;
                    float v2x = v_2.x() - center_x;
                    float v2y = v_2.y() - center_y;
                    float AB = v1x * v0y - v1y * v0x;
                    float BC = v2x * v1y - v2y * v1x;
                    float CA = v0x * v2y - v0y * v2x;
                    if (AB > 0 && BC > 0 && CA > 0)
                    {
                        float S = 1.0f / (AB + BC + CA);
                        float u = BC * S;
                        float v = CA * S;
                        float w = AB * S;
                        float depth = lerp(v_0.z(), v_1.z(), v_2.z(), u, v, w);
                        if (depth < shadowMap->map[index + j])
                        {
                            shadowMap->map[index + j] = depth;
                        }
                    }
                }
            }
        }
        delete[] point_2d;
    }
    //    iodata::write_depth_image(this);
}

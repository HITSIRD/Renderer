//
// Created by 闻永言 on 2021/8/28.
//

#include "Light.hpp"
#include "Util.hpp"
#include "iodata.hpp"

//Light::Light(
//        int _pixel_x, int _pixel_y, float _ccd_size_x, float _ccd_size_y, float _focal, float _x, float _y, float _z,
//        float _h, float _p, float _b):x(_pixel_x), y(_pixel_y), pitch(_h), yaw(_p), roll(_b)
//{
//    luminance = 1.0f;
//
//    center << _x / 100.0f, -_y / 100.0f, _z / 100.0f, 0; // left system to right system
//    FovH = 2 * atanf(_ccd_size_x / (2 * _focal));
//    FovV = 2 * atanf(_ccd_size_y / (2 * _focal));
//    calculate_R();
//
//    vec3 w_c(center.x(), center.y(), center.z());
//    l_t3 = -(l_R3 * w_c);
//    RowVector4f term_0(0, 0, 0, 1);
//    M_view << l_R3, l_t3, term_0;
//
//    Q = M_view.inverse().transpose();
//
//    n = _focal / 1000.0f; // mm to m
//    f = 1000.0f; // far is_clip plane
//    r = f * tanf(FovH / 2.0f);
//    l = -r;
//    t = f * tanf(FovV / 2.0f);
//    b = -t;
//
//    M_per << 2.0f * f / (r - l), 0, 0, 0, 0, 2.0f * f / (t - b), 0, 0, 0, 0, f / (f - n), f * n /
//                                                                                          (n - f), 0, 0, 1.0f, 0;
//
//    M_orth << 2.0f / (r - l), 0, 0, (l + r) / (l - r), 0, 2.0f / (t - b), 0, (b + t) / (b - t), 0, 0, 1.0f / (f - n),
//            n / (n - f), 0, 0, 0, 1.0f;
//
//    M_viewport << (float)_pixel_x * 0.5f, 0, 0, (float)_pixel_x * 0.5f, 0, (float)_pixel_y * 0.5f, 0, (float)_pixel_y *
//                                                                                                  0.5f, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f;
//
//    P = M_viewport * M_per * M_view;
//    O = M_viewport * M_orth * M_view;
//    shadow_map = nullptr;
//}

Light::~Light() = default;

void Light::shadow_mapping(Model *model){}

PointLight::PointLight(float _luminance, int _pixel_size, float _x, float _y, float _z)
{
    type = POINT;
    luminance = _luminance;
    shadow_size = _pixel_size;

    center << _x / 100.0f, -_y / 100.0f, _z / 100.0f, 1.0f; // left system to right system
    Eigen::RowVector4f term_0(0, 0, 0, 1.0f);

    shadow_map = nullptr;
}

void PointLight::shadow_mapping(Model *model)
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

void SunLight::set_luminance(float _luminance)
{
    luminance = _luminance;
}

void SunLight::set_viewport(int _x, int _y, float ccd_size_x, float ccd_size_y, float focal)
{
    x = _x;
    y = _y;
    float FovH = 2 * atanf(ccd_size_x / (2 * focal));
    float FovV = 2 * atanf(ccd_size_y / (2 * focal));

    n = focal / 1000.0f; // mm to m
    f = 10000.0f; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    M_orth << 2.0f / (r - l), 0, 0, (l + r) / (l - r), 0, 2.0f / (t - b), 0, (b + t) / (b - t), 0, 0, 1.0f / (f - n),
            n / (n - f), 0, 0, 0, 1.0f;

    M_view << (float)x * 0.5f, 0, 0, (float)x * 0.5f, 0, (float)y * 0.5f, 0, (float)y *
                                                                             0.5f, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f;
}

void SunLight::set_viewport(int _x, int _y, float range_x)
{
    x = _x;
    y = _y;

    n = 0.01f; // near clip plane
    f = 10000.0f; // far clip plane
    float r = range_x * 0.5f;
    float l = -r;
    float t = range_x * 0.5f * (float)y / (float)x;
    float b = -t;

    M_orth << 2.0f / (r - l), 0, 0, (l + r) / (l - r), 0, 2.0f / (t - b), 0, (b + t) / (b - t), 0, 0, 1.0f / (f - n),
            n / (n - f), 0, 0, 0, 1.0f;

    M_view << (float)x * 0.5f, 0, 0, (float)x * 0.5f, 0, (float)y * 0.5f, 0, (float)y *
                                                                             0.5f, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f;
}

void SunLight::set_look_at(float4 light_center, float4 _focal_center, float4 _up)
{
    center = std::move(light_center);
    up = std::move(_up);
    float4 Z = (_focal_center - center).normalized();
    direct = -Z;
    float4 X = Z.cross3(up).normalized();
    float4 Y = Z.cross3(X).normalized();
    float4 temp(0, 0, 0, 0);
    M_cam << X, Y, Z, temp;
    M_cam.transposeInPlace();
    float4 translation(-X.dot(center), -Y.dot(center), -Z.dot(center), 1.0f);
    M_cam.col(3) = translation;

    MO = M_view * M_orth * M_cam;
}

void SunLight::shadow_mapping(Model *model)
{
    delete shadow_map;
    shadow_map = new ShadowMap(x, y);

    for (const auto &mesh: model->meshes)
    {
        auto *point_2d = new float4[mesh->num_vertex];
        // Calculate depth, back projection point_2d coordinate of every vertex
        for (int i = 0; i < mesh->num_vertex; i++)
        {
            point_2d[i] = MO * mesh->vertices[i].position;
        }

#pragma omp parallel for
        for (const auto &triangle: mesh->triangles)
        {
            float4 v_0 = point_2d[triangle.vertex_0];
            float4 v_1 = point_2d[triangle.vertex_1];
            float4 v_2 = point_2d[triangle.vertex_2];

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
                        if (depth < shadow_map->map[index + j])
                        {
                            shadow_map->map[index + j] = depth;
                        }
                    }
                }
            }
        }
        delete[] point_2d;
    }
    //    iodata::write_depth_image(this);
}

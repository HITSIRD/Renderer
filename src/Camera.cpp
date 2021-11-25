//
// Created by 闻永言 on 2021/7/10.
//

#include "Camera.hpp"
#include "Eigen/Geometry"
#include "Util.hpp"

Camera::~Camera() = default;

void Camera::set_viewport(int _x, int _y, float ccd_size_x, float ccd_size_y, float _focal)
{
    x = _x;
    y = _y;
    ccd_x = ccd_size_x;
    ccd_y = ccd_size_y;
    focal = _focal;
    FovH = 2.0f * atanf(ccd_size_x / (2.0f * focal));
    FovV = 2.0f * atanf(ccd_size_y / (2.0f * focal));

    n = focal / 100.0f; // mm to m
    f = 10000.0f; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    M_per << 2.0f * f / (r - l), 0, 0, 0,
            0,                  2.0f * f / (t - b), 0, 0,
            0,                  0, f / (f - n), f * n / (n - f),
            0,                  0, 1.0f, 0;

    M_orth << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
            0,              2.0f / (t - b), 0, (b + t) / (b - t),
            0,              0, 1.0f / (f - n), n / (n - f),
            0,              0, 0, 1.0f;

    M_viewport << (float)x * 0.5f, 0, 0, (float)x * 0.5f,
            0, (float)y * 0.5f, 0, (float)y * 0.5f,
            0,                     0, 1.0f, 0,
            0,                     0, 0, 1.0f;
}

void Camera::set_viewport(int _x, int _y, float Fov)
{
    x = _x;
    y = _y;
    FovH = Fov;
    FovV = 2.0f * atanf(tanf(deg2rad(Fov) * 0.5f) * (float)y / (float)x);

    n = tanf(Fov * 0.5f) / 1000.0f; // mm to m
    f = 10000.0f; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    M_per << 2.0f * f / (r - l), 0, 0, 0,
            0,                  2.0f * f / (t - b), 0, 0,
            0,                  0, f / (f - n), f * n / (n - f),
            0,                  0, 1.0f, 0;

    M_orth << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
            0,              2.0f / (t - b), 0, (b + t) / (b - t),
            0,              0, 1.0f / (f - n), n / (n - f),
            0,              0, 0, 1.0f;

    M_viewport << (float)x * 0.5f, 0, 0, (float)x * 0.5f,
            0, (float)y * 0.5f, 0, (float)y * 0.5f,
            0,                     0, 1.0f, 0,
            0,                     0, 0, 1.0f;
}

void Camera::set_look_at(float4 _camera_center, float4 _focal_center, float4 _up, int mode)
{
    if(mode == Render::RIGHT)
    {
        camera_center = std::move(_camera_center);
        focal_center = std::move(_focal_center);
        up = std::move(_up);
        float4 Z = (focal_center - camera_center).normalized();
        float4 X = Z.cross3(up).normalized();
        float4 Y = Z.cross3(X).normalized();
        float4 temp(0, 0, 0, 0);
        M_view << X, Y, Z, temp;
        M_view.transposeInPlace();
        float4 translation(-X.dot(camera_center), -Y.dot(camera_center), -Z.dot(camera_center), 1.0f);
        M_view.col(3) = translation;

        VP = M_per * M_view;
        P = M_viewport * VP;
        O = M_viewport * M_orth * M_view;
        Q = M_view.inverse().transpose(); // normal vector transformation matrix
    }
    else if(mode == Render::LEFT)
    {
        _camera_center.y() = -_camera_center.y();
        _focal_center.y() = -_focal_center.y();
        _up.y() = -_up.y();
        set_look_at(_camera_center, _focal_center, _up, Render::RIGHT);
    }
}

void Camera::set_look_at(float4 _camera_center, float h, float p, float b, int mode)
{
    if(mode == Render::RIGHT)
    {

    }
    else if(mode == Render::LEFT)
    {

    }
}

void Camera::update()
{
    //    calculate_R();
    //
    //    vec3 w_c(center.x(), center.y(), center.z());
    //    l_t3 = -(l_R3 * w_c);
    //    RowVector4f term_0(0, 0, 0, 1);
    //    M_view << l_R3, l_t3, term_0;
    //
    //    Q = M_view.inverse().transpose();
}

void Camera::calculate_HPB(float3x3 &H, float3x3 &P, float3x3 &B) const
{
//    float h_rad = -deg2rad(pitch);
//    float p_rad = -deg2rad(yaw);
//    float b_rad = -deg2rad(roll);
//
//    H << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad);
//    P << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad);
//    B << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1;
}

//mat3 Camera::calculate_R()
//{
//    mat3 H, P, B, S_y;
//    calculate_HPB(H, P, B);
//    S_y << 1, 0, 0, 0, -1, 0, 0, 0, 1;
//    mat3 R3 = S_y * H * P * B * S_y;
//    R3.transposeInPlace();
//    Eigen::RowVector4f term_0(0, 0, 0, 1);
//    vec3 term_1(0, 0, 0);
//    return R3;
//}

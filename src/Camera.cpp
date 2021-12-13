//
// Created by 闻永言 on 2021/7/10.
//

#include "Camera.hpp"
#include "iodata.hpp"
#include "Eigen/Geometry"
#include "Util.hpp"

using namespace Render;

Camera::~Camera() = default;

void Camera::setViewport(int _x, int _y, float ccdSizeX, float ccdSizeY, float _focal)
{
    x = _x;
    y = _y;
    ccdX = ccdSizeX;
    ccdY = ccdSizeY;
    focalLength = _focal;
    FovH = 2.0f * atanf(ccdSizeX / (2.0f * focalLength));
    FovV = 2.0f * atanf(ccdSizeY / (2.0f * focalLength));

    n = focalLength / 1000.0f / SCALE; // mm to m
    f = 10000.0f / SCALE; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    matrixPerspective << 2.0f * f / (r - l), 0, 0, 0,
            0,                  2.0f * f / (t - b), 0, 0,
            0,                  0, f / (f - n), f * n / (n - f),
            0,                  0, 1.0f, 0;

    matrixOrthographic << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
            0,              2.0f / (t - b), 0, (b + t) / (b - t),
            0,              0, 1.0f / (f - n), n / (n - f),
            0,              0, 0, 1.0f;

    matrixViewport << (float)x * 0.5f, 0, 0, (float)x * 0.5f,
            0, (float)y * 0.5f, 0, (float)y * 0.5f,
            0,                     0, 1.0f, 0,
            0,                     0, 0, 1.0f;
}

void Camera::setViewport(int _x, int _y, float Fov)
{
    x = _x;
    y = _y;
    FovH = Fov;
    FovV = 2.0f * atanf(tanf(deg2rad(Fov) * 0.5f) * (float)y / (float)x);

    n = tanf(Fov * 0.5f) / 10.0f / SCALE; // mm to m
    f = 10000.0f / SCALE; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    matrixPerspective << 2.0f * f / (r - l), 0, 0, 0,
            0,                  2.0f * f / (t - b), 0, 0,
            0,                  0, f / (f - n), f * n / (n - f),
            0,                  0, 1.0f, 0;

    matrixOrthographic << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
            0,              2.0f / (t - b), 0, (b + t) / (b - t),
            0,              0, 1.0f / (f - n), n / (n - f),
            0,              0, 0, 1.0f;

    matrixViewport << (float)x * 0.5f, 0, 0, (float)x * 0.5f,
            0, (float)y * 0.5f, 0, (float)y * 0.5f,
            0,                     0, 1.0f, 0,
            0,                     0, 0, 1.0f;
}

void Camera::setLookAt(float4 _position, float4 _focal, float4 _up, SYSTEM system)
{
    if(system == RIGHT)
    {
        position = std::move(_position);
        focal = std::move(_focal);
        _up = std::move(_up);
        float4 Z = (focal - position).normalized();
        float4 X = Z.cross3(_up).normalized();
        float4 Y = Z.cross3(X).normalized();
        float4 temp(0, 0, 0, 0);
        matrixView << X, Y, Z, temp;
        matrixView.transposeInPlace();
        float4 translation(-X.dot(position), -Y.dot(position), -Z.dot(position), 1.0f);
        matrixView.col(3) = translation;

        VP = matrixPerspective * matrixView;
        P = matrixViewport * VP;
        matrixScreenToWorld = P.inverse();
        O = matrixViewport * matrixOrthographic * matrixView;
        Q = matrixView.inverse().transpose(); // normal vector transformation matrix
    }
    else if(system == LEFT)
    {
        _position.y() = -_position.y();
        _focal.y() = -_focal.y();
        _up.y() = -_up.y();
        setLookAt(_position, _focal, _up, RIGHT);
    }
}

void Camera::setLookAt(float4 _position, float h, float p, float b, SYSTEM system)
{
//    if(mode == Render::RIGHT)
//    {
//
//    }
//    else if(mode == Render::LEFT)
//    {
//
//    }
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

void Camera::calculateHPB(float3x3 &H, float3x3 &P, float3x3 &B) const
{
//    float h_rad = -deg2rad(pitch);
//    float p_rad = -deg2rad(yaw);
//    float b_rad = -deg2rad(roll);
//
//    H << cos(h_rad), 0, sin(h_rad), 0, 1, 0, -sin(h_rad), 0, cos(h_rad);
//    P << 1, 0, 0, 0, cos(p_rad), -sin(p_rad), 0, sin(p_rad), cos(p_rad);
//    B << cos(b_rad), -sin(b_rad), 0, sin(b_rad), cos(b_rad), 0, 0, 0, 1;
}

//float3x3 Camera::calculate_R()
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

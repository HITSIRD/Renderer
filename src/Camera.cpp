//
// Created by 闻永言 on 2021/7/10.
//

#include "Camera.hpp"
#include "Eigen/Geometry"
#include "Util.hpp"

using namespace Renderer;

Camera::~Camera() = default;

void Camera::setViewport(int _x, int _y, float ccdSizeX, float ccdSizeY, float _focal) {
    x = _x;
    y = _y;
    ccdX = ccdSizeX;
    ccdY = ccdSizeY;
    focalLength = _focal;
    FovH = 2.0f * atanf(ccdSizeX / (2.0f * focalLength));
    FovV = 2.0f * atanf(ccdSizeY / (2.0f * focalLength));

    n = focalLength / 1000.0f; // mm to m
    f = 20000.0f; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    matrixPerspective << 2.0f * f / (r - l), 0, 0, 0,
            0, 2.0f * f / (t - b), 0, 0,
            0, 0, f / (f - n), f * n / (n - f),
            0, 0, 1.0f, 0;

    matrixOrthographic << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
            0, 2.0f / (t - b), 0, (b + t) / (b - t),
            0, 0, 1.0f / (f - n), n / (n - f),
            0, 0, 0, 1.0f;

    matrixViewport << (float) x * 0.5f, 0, 0, (float) x * 0.5f,
            0, (float) y * 0.5f, 0, (float) y * 0.5f,
            0, 0, 1.0f, 0,
            0, 0, 0, 1.0f;

    matrixScreenToView = (matrixViewport * matrixPerspective).inverse();
}

void Camera::setViewport(int _x, int _y, float Fov) {
    x = _x;
    y = _y;
    FovH = Fov;
    FovV = 2.0f * atanf(tanf(deg2rad(Fov) * 0.5f) * (float) y / (float) x);

    n = tanf(Fov * 0.5f) / 10.0f; // mm to m
    f = 20000.0f; // far is_clip plane
    float r = f * tanf(FovH * 0.5f);
    float l = -r;
    float t = f * tanf(FovV * 0.5f);
    float b = -t;

    matrixPerspective << 2.0f * f / (r - l), 0, 0, 0,
            0, 2.0f * f / (t - b), 0, 0,
            0, 0, f / (f - n), f * n / (n - f),
            0, 0, 1.0f, 0;

    matrixOrthographic << 2.0f / (r - l), 0, 0, (l + r) / (l - r),
            0, 2.0f / (t - b), 0, (b + t) / (b - t),
            0, 0, 1.0f / (f - n), n / (n - f),
            0, 0, 0, 1.0f;

    matrixViewport << (float) x * 0.5f, 0, 0, (float) x * 0.5f,
            0, (float) y * 0.5f, 0, (float) y * 0.5f,
            0, 0, 1.0f, 0,
            0, 0, 0, 1.0f;

    matrixScreenToView = (matrixViewport * matrixPerspective).inverse();
}

void Camera::setLookAt(float4 _position, float4 _focal, float4 _up, System system) {
    if (system == RIGHT) {
        position = std::move(_position);
        focal = std::move(_focal);
        up = std::move(_up);
        float4 Z = (focal - position).normalized();
        float4 X = Z.cross3(up).normalized();
        float4 Y = Z.cross3(X).normalized();
        float4 temp(0, 0, 0, 0);
        matrixView << X, Y, Z, temp;
        matrixView.transposeInPlace(); // inverse equals transpose
        float4 translation(-X.dot(position), -Y.dot(position), -Z.dot(position), 1.0f);
        matrixView.col(3) = translation;

        matrixVP = matrixPerspective * matrixView;
        matrixWorldToScreen = matrixViewport * matrixVP;
        matrixNormal = matrixView.inverse().transpose(); // normal vector transformation matrix
        matrixViewToWorld = matrixView.inverse();
    } else if (system == LEFT) {
        _position.y() = -_position.y();
        _focal.y() = -_focal.y();
        _up.y() = -_up.y();
        setLookAt(_position, _focal, _up, RIGHT);
    }
}

void Camera::setLookAt(float4 _position, float yaw, float pitch, float roll, EulerAngleOrder order, System system) {
    if (system == RIGHT) {
        position = std::move(_position);
        float3x3 matrixRotate;
        switch (order) {
            case ZYX:
                matrixRotate = Eigen::AngleAxisf(deg2rad(roll), float3::UnitZ()) *
                               Eigen::AngleAxisf(deg2rad(pitch), float3::UnitY()) *
                               Eigen::AngleAxisf(deg2rad(yaw), float3::UnitX());
                break;
            case ZXY:
                matrixRotate = Eigen::AngleAxisf(deg2rad(roll), float3::UnitZ()) *
                               Eigen::AngleAxisf(deg2rad(pitch), float3::UnitX()) *
                               Eigen::AngleAxisf(deg2rad(yaw), float3::UnitY());
                break;
            case YXZ:
                matrixRotate = Eigen::AngleAxisf(deg2rad(roll), float3::UnitY()) *
                               Eigen::AngleAxisf(deg2rad(pitch), float3::UnitX()) *
                               Eigen::AngleAxisf(deg2rad(yaw), float3::UnitZ());
                break;
            case YZX:
                matrixRotate = Eigen::AngleAxisf(deg2rad(roll), float3::UnitY()) *
                               Eigen::AngleAxisf(deg2rad(pitch), float3::UnitZ()) *
                               Eigen::AngleAxisf(deg2rad(yaw), float3::UnitX());
                break;
            case XYZ:
                matrixRotate = Eigen::AngleAxisf(deg2rad(roll), float3::UnitX()) *
                               Eigen::AngleAxisf(deg2rad(pitch), float3::UnitY()) *
                               Eigen::AngleAxisf(deg2rad(yaw), float3::UnitZ());
                break;
            case XZY:
                matrixRotate = Eigen::AngleAxisf(deg2rad(roll), float3::UnitX()) *
                               Eigen::AngleAxisf(deg2rad(pitch), float3::UnitZ()) *
                               Eigen::AngleAxisf(deg2rad(yaw), float3::UnitY());
                break;
        }
        float3 pos(position[0], position[1], position[2]);
        float3 translation = -matrixRotate * pos;
        Eigen::RowVector4f temp(0, 0, 0, 1.f);
        matrixView << matrixRotate, translation, temp;

        matrixVP = matrixPerspective * matrixView;
        matrixWorldToScreen = matrixViewport * matrixVP;
        matrixNormal = matrixView.inverse().transpose(); // normal vector transformation matrix
        matrixViewToWorld = matrixView.inverse();
    } else if (system == LEFT) {
        _position.y() = -_position.y();
        setLookAt(_position, yaw, pitch, roll, order, RIGHT);
    }
}

void Camera::setLookAt(float4 _position, float4 quaternion, Renderer::System system) {
    if (system == RIGHT) {
        position = std::move(_position);
        matrixView << 1 - 2 * quaternion.y() * quaternion.y() - 2 * quaternion.z() * quaternion.z(),
                2 * quaternion.x() * quaternion.y() - 2 * quaternion.z() * quaternion.w(),
                2 * quaternion.x() * quaternion.z() + 2 * quaternion.y() * quaternion.w(), 0,
                2 * quaternion.x() * quaternion.y() + 2 * quaternion.z() * quaternion.w(), 1 -
                2 * quaternion.x() * quaternion.x() - 2 * quaternion.z() * quaternion.z(),
                2 * quaternion.y() * quaternion.z() - 2 * quaternion.x() * quaternion.w(), 0,
                2 * quaternion.x() * quaternion.z() - 2 * quaternion.y() * quaternion.w(),
                2 * quaternion.y() * quaternion.z() + 2 * quaternion.x() * quaternion.w(), 1 -
                2 * quaternion.x() * quaternion.x() - 2 * quaternion.y() * quaternion.y(), 0, 0, 0, 0, 1.0f;
        matrixView.col(3) = -matrixView * position;

        matrixVP = matrixPerspective * matrixView;
        matrixWorldToScreen = matrixViewport * matrixVP;
        matrixNormal = matrixView.inverse().transpose(); // normal vector transformation matrix
        matrixViewToWorld = matrixView.inverse();
    } else if (system == LEFT) {
        _position.y() = -_position.y();
        setLookAt(_position, quaternion, RIGHT);
    }
}

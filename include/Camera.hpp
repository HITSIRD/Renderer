//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_CAMERA_HPP
#define RENDERER_CAMERA_HPP

#include "Type.hpp"

namespace Renderer
{
    enum System
    {
        RIGHT = 0,
        LEFT = 1
    };

    enum EulerAngleOrder
    {
        ZYX,
        ZXY,
        YXZ,
        YZX,
        XYZ,
        XZY
    };
}

class Camera
{
public:
    // camera inner parameters
    int x; // pixel number in x
    int y; // pixel number in y

    // camera physical parameters, can be ignored
    float ccdX; // ccd size in x
    float ccdY; // ccd size in y
    float focalLength; // (mm)

    float4 position; // camera coordinate in world space
    float4 focal;
    float4 up;
    float FovH; // field of view
    float FovV; // field of view

    float n; // near is_clip plane
    float f; // far is_clip plane

    float4x4 matrixView; // world space to camera space matrix
    float4x4 matrixPerspective; // perspective projection transformation matrix
    float4x4 matrixOrthographic; // orthographic projection transformation matrix
    float4x4 matrixViewport; // normal device space to screen space matrix
    float4x4 matrixNormal; // normal vector transformation matrix

    // ray tracing
    float4x4 matrixScreenToView; // (matrixPerspective * matrixViewport).inverse()
    float4x4 matrixViewToWorld; // matrixView.inverse()

    float4x4 matrixWorldToScreen; // world to screen perspective transformation
    float4x4 matrixVP; // MVP matrix

    /**
     *
     */
    Camera()
    {}

    /**
     * destructor
     */
    ~Camera();

    /**
     * Set camera viewport parameters.
     * @param _x window width
     * @param _y window height
     * @param ccdSizeX ccd size in x
     * @param ccdSizeY ccd size in y
     * @param _focal focal of camera
     */
    void setViewport(int _x, int _y, float ccdSizeX, float ccdSizeY, float _focal);

    /**
     * Set camera viewport parameters
     * @param _x window width
     * @param _y window height
     * @param Fov FovV, FovH will be automatically set in window scale
     */
    void setViewport(int _x, int _y, float Fov);

    /**
     * Set view space transformation matrix.
     * @param _position camera center coordination
     * @param _focal focal center coordination
     * @param _up look up vector
     * @param system right system (0), left system (1)
     */
    void setLookAt(float4 _position, float4 _focal, float4 _up, Renderer::System system = Renderer::RIGHT);

    /**
     *
     * @param _position
     * @param h
     * @param p
     * @param b
     * @param order
     * @param system right hand system or left hand system
     */
    void setLookAt(
            float4 _position, float yaw, float pitch, float roll, Renderer::EulerAngleOrder order = Renderer::ZYX,
            Renderer::System system = Renderer::RIGHT);

    /**
     *
     * @param _position
     * @param quaternion
     * @param system
     */
    void setLookAt(float4 _position, float4 quaternion, Renderer::System system = Renderer::RIGHT);

private:
};

#endif

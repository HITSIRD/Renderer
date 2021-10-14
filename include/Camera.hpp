//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_CAMERA_HPP
#define RENDERER_CAMERA_HPP

#include "Type.hpp"

namespace Render
{
    enum
    {
        RIGHT = 0,
        LEFT = 1
    };
}

class Camera
{
public:
    // camera inner parameters
    int x; // pixel number in x
    int y; // pixel number in y

    // camera physical parameters, can be ignored
    float ccd_x; // ccd size in x
    float ccd_y; // ccd size in y
    float focal; // (mm)

    float4 camera_center; // camera coordinate in world space
    float4 focal_center;
    float4 up;
    float FovH; // field of view
    float FovV; // field of view

    float n; // near is_clip plane
    float f; // far is_clip plane

    float4x4 M_view; // world space to camera space matrix
    float4x4 M_per; // perspective projection transformation matrix
    float4x4 M_orth; // orthographic projection transformation matrix
    float4x4 M_viewport; // normal device space to screen space matrix
    float4x4 Q; // normal vector transformation matrix

    float4x4 P;
    float4x4 O;
    float4x4 VP;

    /**
     *
     */
    Camera(){}

    /**
     * destructor
     */
    ~Camera();

    /**
     * Set camera viewport parameters.
     * @param _x window width
     * @param _y window height
     * @param ccd_size_x ccd size in x
     * @param ccd_size_y ccd size in y
     * @param _focal focal of camera
     */
    void set_viewport(int _x, int _y, float ccd_size_x, float ccd_size_y, float _focal);

    /**
     * Set camera viewport parameters
     * @param _x window width
     * @param _y window height
     * @param Fov FovV, FovH will be automatically set in window scale
     */
    void set_viewport(int _x, int _y, float Fov);

    /**
     * Set view space transformation matrix.
     * @param _camera_center camera center coordination
     * @param _focal_center focal center coordination
     * @param up look up vector
     * @param mode right system (0), left system (1)
     */
    void set_look_at(float4 _camera_center, float4 _focal_center, float4 up, int mode = Render::RIGHT);

    /**
     *
     * @param _camera_center
     * @param h
     * @param p
     * @param b
     * @param mode right system (0), left system (1)
     */
    void set_look_at(float4 _camera_center, float h, float p, float b, int mode = Render::RIGHT);

    /**
     *
     * @param direction
     * @param angle
     */
    void rotate(float4 direction, float angle);

    /**
     * Convert the c4d left-handed world coordinate to camera right-handed coordinate.
     * Update the parameters.
     */
    void update();
private:
    /**
    * Calculate rotation matrix of H, P, B.
    * @param H
    * @param P
    * @param B
    */
    void calculate_HPB(float3x3 &H, float3x3 &P, float3x3 &B) const;

    /**
     * Calculate rotation matrix l_R3.
     */
    float3x3 calculate_R();
};

#endif

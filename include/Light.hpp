//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_LIGHT_HPP
#define RENDERER_LIGHT_HPP

#include "Type.hpp"
#include "Model.hpp"

enum LIGHT_TYPE
{
    BASE, POINT, SUN
};

class ShadowMap
{
public:
    int x;
    int y;
    float *map;

    /**
     *
     * @param _x
     * @param _y
     */
    ShadowMap(int _x, int _y):x(_x), y(_y)
    {
        map = new float[x * y];
        for (int i = 0; i < x * y; i++)
        {
            map[i] = 1.0f;
        }
    }

    ~ShadowMap()
    {
        if(map)
        {
            delete[] map;
        }
    }
};

class Light
{
public:
    LIGHT_TYPE type;
    float luminance;

    float4 center; // light coordinate in world space

    ShadowMap *shadow_map;

    Light():type(SUN), luminance(1.0f), center(1.0f, 1.0f, 1.0f, 1.0f), shadow_map(nullptr){}

    ~Light();

    /**
     *
     * @param model
     */
    virtual void shadow_mapping(Model *model);
};

class PointLight:public Light
{
public:
    int shadow_size; // shadow map size
    float4 center; // light coordinate in world space
    //    float FovH; // field of view
    //    float FovV; // field of view
    //
    //    float l; // left is_clip plane
    //    float r; // right is_clip plane
    //    float t; // top is_clip plane
    //    float b; // bottom is_clip plane
    //    float n; // near is_clip plane
    //    float f; // far is_clip plane
    //
    //    // c4d euler angle parameters
    //    float pitch; // h
    //    float yaw; // p
    //    float roll; // b
    //
    //    mat3 R3; // temp
    //    vec3 t3; // temp
    //
    //    mat4 R; // world space to camera space rotation matrix
    //    mat4 M_view; // world space to camera space matrix
    //    mat4 Q; // normal vector transformation matrix
    //    mat4 M_per; // perspective projection transformation matrix
    //    mat4 M_orth; // orthographic projection transformation matrix
    //    mat4 M_viewport; // normal device space to screen space matrix
    //
    //    mat4 P;

    /**
     *
     * @param luminance
     * @param _pixel_size
     * @param _x
     * @param _y
     * @param _z
     */
    PointLight(float luminance, int _pixel_size, float _x, float _y, float _z);

    void shadow_mapping(Model *model) override;
};

class SunLight:public Light
{
public:
    int x; // pixel number in x
    int y; // pixel number in y
    float4 up;

    float4 direct; // direct light

    float n; // near is_clip plane
    float f; // far is_clip plane

    float4x4 M_cam; // world space to camera space matrix
    float4x4 M_orth; // orthographic projection transformation matrix
    float4x4 M_view; // normal device space to screen space matrix

    float4x4 MO;

    //    SunLight(vec4 direct_light):direct(direct_light){}

    SunLight()
    {
        type = SUN;
        shadow_map = nullptr;
    }

    /**
     *
     * @param _luminance
     */
    void set_luminance(float _luminance);

    /**
     * Set camera viewport parameters.
     * @param _x window width
     * @param _y window height
     * @param ccd_size_x ccd size in x
     * @param ccd_size_y ccd size in y
     * @param focal focal of camera
     */
    void set_viewport(int _x, int _y, float ccd_size_x, float ccd_size_y, float focal);

    /**
     * Set camera viewport parameters
     * @param _x window width
     * @param _y window height
     * @param range_x orthographic projection range in x
     */
    void set_viewport(int _x, int _y, float range_x);

    /**
     * Set view space transformation matrix.
     * @param light_center light center coordination
     * @param _focal_center focal center coordination
     * @param up look up vector
     */
    void set_look_at(float4 light_center, float4 _focal_center, float4 up);

    /**
     *
     * @param model
     */
    void shadow_mapping(Model *model) override;
};

#endif //RENDERER_LIGHT_HPP

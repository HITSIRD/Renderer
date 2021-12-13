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

    void reset() const
    {
        if(map)
        {
            for (int i = 0; i < x * y; i++)
            {
                map[i] = 1.0f;
            }
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
    float intensity;

    float4 position; // light coordinate in world space

    ShadowMap *shadowMap;

    Light():type(SUN), intensity(1.0f), position(0, 0, 0, 1.0f), shadowMap(nullptr){}

    ~Light();

    /**
     *
     * @param model
     */
    virtual void shadowMapping(Model *model);
};

class PointLight:public Light
{
public:
    int shadowSize; // shadow map size
    //
    //    float n; // near is_clip plane
    //    float f; // far is_clip plane
    //
    //    mat4 P;

    /**
     *
     * @param power
     * @param _shadowSize
     * @param _position
     */
    PointLight(float power, int _shadowSize, float4 _position);

    void shadowMapping(Model *model) override;
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

    float4x4 matrixView; // world space to camera space matrix
    float4x4 matrixOrthographic; // orthographic projection transformation matrix
    float4x4 matrixViewport; // normal device space to screen space matrix

    float4x4 matrixWorldToScreen;

    //    SunLight(vec4 direct_light):direct(direct_light){}

    SunLight()
    {
        type = SUN;
        shadowMap = nullptr;
    }

    /**
     *
     * @param _intensity
     */
    void setIntensity(float _intensity);

    /**
     * Set camera viewport parameters.
     * @param _x window width
     * @param _y window height
     * @param ccdSizeX ccd size in x
     * @param ccdSizeY ccd size in y
     * @param focalLength focal of camera
     */
    void setViewport(int _x, int _y, float ccdSizeX, float ccdSizeY, float focalLength);

    /**
     * Set camera viewport parameters
     * @param _x window width
     * @param _y window height
     * @param range orthographic projection range
     */
    void setViewport(int _x, int _y, float range);

    /**
     * Set view space transformation matrix.
     * @param _position light center coordination
     * @param _focal focal center coordination
     * @param _up look up vector
     */
    void setLookAt(float4 _position, float4 _focal, float4 _up);

    /**
     *
     * @param model
     */
    void shadowMapping(Model *model) override;
};

#endif //RENDERER_LIGHT_HPP

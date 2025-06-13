//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_LIGHT_HPP
#define RENDERER_LIGHT_HPP

#include "Type.hpp"
#include "Model.hpp"
#include "Texture.hpp"

class ShadowShader;

namespace Renderer {
    enum LIGHT_TYPE {
        POINT, SUN
    };
}

class Light {
public:
    Renderer::LIGHT_TYPE type;
    int shadowSize; // resolution size \times size
    float intensity;
    float4 color;
    float4 position; // light coordinate in world space

    ShadowShader *shader;
    bool updateShadow; // if update shadow map

    Light();

    ~Light();
};

class PointLight : public Light {
public:
    float4x4 matrixWorldToScreen[6];
    float4x4 VP[6]; // MVP matrix
    float4x4 matrixViewport;

    TextureCube<float> *shadowMap;

    /**
     *
     * @param power
     * @param _shadowSize
     * @param _position
     */
    PointLight(float power, int _shadowSize, float4 _position);

    /**
     *
     * @param power
     * @param _shadowSize
     * @param position
     */
    void setup(float power, int _shadowSize, float4 _position);

    /**
     * Set shadow map.
     * @param faceIndex
     * @param buffer
     */
    void setShadowMap(int faceIndex, Image<float> *buffer);
};

class SunLight : public Light {
public:
    float4 up;
    float4 direct; // direct light

    float n; // near is_clip plane
    float f; // far is_clip plane

    float4x4 matrixView; // world space to camera space matrix
    float4x4 matrixOrthographic; // orthographic projection transformation matrix
    float4x4 matrixViewport; // normal device space to screen space matrix
    float4x4 matrixWorldToScreen;

    Texture2D<float> *shadowMap;

    SunLight();

    /**
     *
     * @param _intensity
     */
    void setIntensity(float _intensity);

    /**
     * Set light viewport parameters
     * @param _size resolution, should be 2^N, if not, convert explicitly
     * @param range orthographic projection range
     */
    void setViewport(int _size, float range);

    /**
     * Set view space transformation matrix.
     * @param _position light center coordination
     * @param _focal focal center coordination
     * @param _up look up vector
     */
    void setLookAt(float4 _position, float4 _focal, float4 _up);

    /**
     * Set shadow map.
     * @param buffer
     */
    void setShadowMap(Image<float> *buffer);
};

#endif //RENDERER_LIGHT_HPP

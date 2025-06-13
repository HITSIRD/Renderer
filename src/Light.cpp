//
// Created by 闻永言 on 2021/8/28.
//

#include "Light.hpp"
#include "Util.hpp"
#include "Eigen/Geometry"
#include <iostream>
#include <utility>

using namespace std;
using namespace Renderer;

Light::Light(): type(SUN), shadowSize(1), intensity(1.0f), color(1.f, 1.f, 1.f, 1.f), position(0, 0, 0, 1.0f),
                shader(nullptr), updateShadow(true) {
}

Light::~Light() = default;

PointLight::PointLight(float power, int _shadowSize, float4 _position): shadowMap(nullptr) {
    setup(power, _shadowSize, std::move(_position));
}

void PointLight::setup(float power, int _shadowSize, float4 _position) {
    type = POINT;
    intensity = power;
    position = std::move(_position); // left system to right system
    delete shadowMap;
    shadowMap = nullptr;

    if ((_shadowSize & (_shadowSize - 1)) != 0) // size must be 2^N
    {
        shadowSize = 1;
        while (_shadowSize != 1) {
            _shadowSize = _shadowSize >> 1;
            shadowSize = shadowSize << 1;
        }
    } else {
        shadowSize = _shadowSize;
    }

    float n = 0.01f;
    float f = 25.f * sqrt(intensity); // far is_clip plane
    float r = f;
    float l = -r;
    float t = r;
    float b = l;

    float4x4 matrixView, matrixPerspective;
    matrixPerspective << 2.f * f / (r - l), 0, 0, 0, 0, 2.f * f / (t - b), 0, 0, 0, 0, f / (f - n), f * n /
            (n - f), 0, 0, 1.0f, 0;
    matrixViewport << (float) shadowSize * 0.5f, 0, 0, (float) shadowSize * 0.5f, 0, (float) shadowSize * 0.5f, 0,
            (float) shadowSize * 0.5f, 0, 0, 1.f, 0, 0, 0, 0, 1.f;
    float4 up[6] = {
        {0, 0, 1.f, 0},
        {0, 0, 1.f, 0},
        {0, 0, 1.f, 0},
        {0, 0, 1.f, 0},
        {1.f, 0, 0, 0},
        {-1.f, 0, 0, 0}
    };
    float4 Z[6] = {
        {0, -1.f, 0, 0},
        {-1.f, 0, 0, 0},
        {0, 1.f, 0, 0},
        {1.f, 0, 0, 0},
        {0, 0, 1.f, 0},
        {0, 0, -1.f, 0}
    };

    for (int i = 0; i < 6; i++) {
        float4 X = Z[i].cross3(up[i]);
        float4 Y = Z[i].cross3(X);
        matrixView << X, Y, Z[i], ZeroFloat4;
        matrixView.transposeInPlace(); // inverse equals transpose
        float4 translation(-X.dot(position), -Y.dot(position), -Z[i].dot(position), 1.f);
        matrixView.col(3) = translation;
        VP[i] = matrixPerspective * matrixView;
        matrixWorldToScreen[i] = matrixViewport * VP[i];
    }
    updateShadow = true; // update shadow map
}

void PointLight::setShadowMap(int faceIndex, Image<float> *buffer) {
    if (!shadowMap) {
        shadowMap = new TextureCube<float>(shadowSize, 1);
    }
    if (buffer->x == shadowMap->getSize() && buffer->y == shadowMap->getSize()) {
        for (int i = 0; i < shadowSize * shadowSize; i++) {
            (*shadowMap)(faceIndex, i) = buffer->data[i];
        }
    }

    updateShadow = false;
}

SunLight::SunLight(): shadowMap(nullptr), n(0.1f), f(10000.f) {
    type = SUN;
}

void SunLight::setIntensity(float _intensity) {
    intensity = _intensity;
}

void SunLight::setViewport(int _size, float range) {
    if ((_size & (_size - 1)) != 0) // size must be 2^N
    {
        shadowSize = 1;
        while (_size != 1) {
            _size = _size >> 1;
            shadowSize = shadowSize << 1;
        }
    } else {
        shadowSize = _size;
    }

    float r = range * 0.5f;
    float l = -r;
    float t = range * 0.5f;
    float b = -t;

    matrixOrthographic << 2.f / (r - l), 0, 0, 0,
            0, 2.f / (t - b), 0, 0,
            0, 0, 1.f / (f - n), n /
            (n - f),
            0, 0, 0, 1.f;
    matrixViewport << (float) shadowSize * 0.5f, 0, 0, (float) shadowSize * 0.5f, 0, (float) shadowSize * 0.5f, 0,
            (float) shadowSize * 0.5f, 0, 0, 1.f, 0, 0, 0, 0, 1.0f;
}

void SunLight::setLookAt(float4 _position, float4 _focal, float4 _up) {
    position = std::move(_position);
    up = std::move(_up);
    float4 Z = (_focal - position).normalized();
    direct = -Z;
    float4 X = Z.cross3(up).normalized();
    float4 Y = Z.cross3(X).normalized();
    matrixView << X, Y, Z, ZeroFloat4;
    matrixView.transposeInPlace();
    float4 translation(-X.dot(position), -Y.dot(position), -Z.dot(position), 1.f);
    matrixView.col(3) = translation;

    matrixWorldToScreen = matrixViewport * matrixOrthographic * matrixView;
    updateShadow = true; // update shadow map
}

void SunLight::setShadowMap(Image<float> *buffer) {
    if (!shadowMap) {
        shadowMap = new Texture2D<float>(buffer);
    } else if (buffer->x == shadowMap->getSize() && buffer->y == shadowMap->getSize()) {
        for (int i = 0; i < shadowSize * shadowSize; i++) {
            (*shadowMap)[i] = buffer->data[i];
        }
    }

    updateShadow = false;
}

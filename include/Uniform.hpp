//
// Created by 闻永言 on 2021/12/23.
//

#ifndef RENDERER_UNIFORM_HPP
#define RENDERER_UNIFORM_HPP

#include "Light.hpp"

class Light;

class Uniform {
public:
    // camera parameters
    float4 viewPosition; // world space
    float4x4 VP; // MVP matrix
    float4x4 matrixViewport;
    float4x4 matrixWorldToScreen;

    std::vector<Light *> *lightSource;

    float ka; // ambient coefficient
    float kd; // diffuse coefficient
    float ks; // specular coefficient
    float specRank;

    Texture2D<unsigned char> *textureBase;
    Texture2D<unsigned char> *textureNormal;
    Texture2D<unsigned char> *textureAO;
    Texture2D<unsigned char> *textureMetalness;
    Texture2D<unsigned char> *textureRoughness;
    Texture2D<unsigned char> *textureEmission;

    bool mipmap;
    bool shadow;
    Renderer::SamplerType samplerType;

    Uniform(): textureBase(nullptr), textureNormal(nullptr), textureAO(nullptr), textureMetalness(nullptr),
               textureRoughness(nullptr), textureEmission(nullptr) {
    }

    Uniform(float _ka, float _kd, float _ks, float _spec): ka(_ka), kd(_kd), ks(_ks), specRank(_spec),
                                                           lightSource(nullptr), samplerType(Renderer::NORMAL),
                                                           textureBase(nullptr), textureNormal(nullptr),
                                                           textureAO(nullptr), textureMetalness(nullptr),
                                                           textureRoughness(nullptr), textureEmission(nullptr) {
    }

    ~Uniform() = default;

    /**
     *
     * @param _viewPosition
     * @param _VP
     * @param _matrixViewport
     * @param _matrixWorldToScreen
     */
    void setup(
        const float4 &_viewPosition, const float4x4 &_VP, const float4x4 &_matrixViewport,
        const float4x4 &_matrixWorldToScreen) {
        viewPosition = std::move(_viewPosition);
        VP = std::move(_VP);
        matrixViewport = std::move(_matrixViewport);
        matrixWorldToScreen = std::move(_matrixWorldToScreen);
    }
};

#endif //RENDERER_UNIFORM_HPP

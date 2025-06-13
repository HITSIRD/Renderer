//
// Created by 闻永言 on 2021/8/28.
//

#include "Material.hpp"

using namespace Renderer;

Shader *Shader::shader;

Material::Material(): shader(nullptr), textureBase(nullptr), textureNormal(nullptr), textureAO(nullptr),
                      textureMetalness(nullptr), textureRoughness(nullptr), textureEmission(nullptr), ambient(0.2f),
                      diffuse(0.7f),
                      specular(0.2f), specRank(16.0f) {
    shader = Shader::getInstance();
}

Material::~Material() = default;

void Material::setShader(Shader *s) {
    shader = s;
}

void Material::setTexture(Texture2D<unsigned char> *texture2D, TextureType type) {
    switch (type) {
        case TEXTURE_BASE:
            textureBase = std::make_shared<Texture2D<unsigned char> >(*texture2D);
            break;
        case TEXTURE_NORMAL:
            textureNormal = std::make_shared<Texture2D<unsigned char> >(*texture2D);
            break;
        case TEXTURE_AO:
            textureAO = std::make_shared<Texture2D<unsigned char> >(*texture2D);
            break;
        case TEXTURE_METALNESS:
            textureMetalness = std::make_shared<Texture2D<unsigned char> >(*texture2D);
            break;
        case TEXTURE_ROUGHNESS:
            textureRoughness = std::make_shared<Texture2D<unsigned char> >(*texture2D);
            break;
        case TEXTURE_EMISSION:
            textureEmission = std::make_shared<Texture2D<unsigned char> >(*texture2D);
            break;
        default:
            break;
    }
}

void Material::setUniform(Uniform &uniform) const {
    uniform.ka = ambient;
    uniform.kd = diffuse;
    uniform.ks = specular;
    uniform.specRank = specRank;

    uniform.textureBase = textureBase.get();
    uniform.textureNormal = textureNormal.get();
    uniform.textureAO = textureAO.get();
    uniform.textureMetalness = textureMetalness.get();
    uniform.textureRoughness = textureRoughness.get();
    uniform.textureEmission = textureEmission.get();
}

Shader *Material::getShader() {
    return shader;
}

void Material::destroy() {
    textureBase.reset();
    textureNormal.reset();
    textureAO.reset();
    textureMetalness.reset();
    textureRoughness.reset();
    textureEmission.reset();
}

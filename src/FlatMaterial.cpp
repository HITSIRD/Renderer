//
// Created by 闻永言 on 2021/8/28.
//

#include "FlatMaterial.hpp"
#include "FlatShader.hpp"

FlatMaterial::FlatMaterial() {
    setShader(FlatShader::instance());
}

void FlatMaterial::destroy() {
    textureBase.reset();
    textureNormal.reset();
    textureAO.reset();
    textureMetalness.reset();
    textureRoughness.reset();
    textureEmission.reset();
}

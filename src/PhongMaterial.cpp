//
// Created by 闻永言 on 2021/8/28.
//

#include "PhongMaterial.hpp"
#include "PhongShader.hpp"

PhongMaterial::PhongMaterial()
{
    setShader(PhongShader::instance());
}

void PhongMaterial::destroy()
{
    textureBase.reset();
    textureNormal.reset();
    textureAO.reset();
    textureMetalness.reset();
    textureRoughness.reset();
    textureEmission.reset();
}

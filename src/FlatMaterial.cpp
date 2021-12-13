//
// Created by 闻永言 on 2021/8/28.
//

#include "FlatMaterial.hpp"
#include "FlatShader.hpp"

FlatMaterial::FlatMaterial(){
    setShader(FlatShader::instance());
}

void FlatMaterial::destroy()
{
    baseTexture.reset();
}

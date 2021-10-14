//
// Created by 闻永言 on 2021/8/28.
//

#include "FlatMaterial.hpp"
#include "FlatShader.hpp"

FlatMaterial::FlatMaterial(){
    set_shader(FlatShader::instance());
}

void FlatMaterial::destroy()
{
    base_texture.reset();
}

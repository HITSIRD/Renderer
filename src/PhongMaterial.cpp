//
// Created by 闻永言 on 2021/8/28.
//

#include "PhongMaterial.hpp"
#include "PhongShader.hpp"

PhongMaterial::PhongMaterial(){
    set_shader(PhongShader::instance());
//    shadow_shader = ShadowShader::instance();
}

void PhongMaterial::destroy()
{
    base_texture.reset();
}

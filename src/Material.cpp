//
// Created by 闻永言 on 2021/8/28.
//

#include "Material.hpp"

Shader *Shader::shader;

Material::Material():shader(nullptr), baseTexture(nullptr), ambient(0.005f), diffuse(0.7f), specular(0.2f), specRank(16.0f)
{
    shader = Shader::instance();
}

Material::~Material() = default;

void Material::setShader(Shader *s)
{
    shader = s;
}

void Material::setTexture(Texture2D *t)
{
    baseTexture = std::make_shared<Texture2D>(*t);
}

Uniform Material::getUniform() const
{
    Uniform u = Uniform(ambient, diffuse, specular, specRank);
    u.baseTexture = baseTexture.get();
    return u;
}

Shader *Material::getShader()
{
    return shader;
}

void Material::destroy(){}

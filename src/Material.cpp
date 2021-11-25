//
// Created by 闻永言 on 2021/8/28.
//

#include "Material.hpp"

Shader *Shader::shader;

Material::Material():shader(nullptr), base_texture(nullptr), ambient(0.005f), diffuse(0.7f), specular(0.2f), spec_rank(16.0f)
{
    shader = Shader::instance();
}

Material::~Material() = default;

void Material::set_shader(Shader *s)
{
    shader = s;
}

void Material::set_texture(Texture2D *t)
{
    base_texture = std::make_shared<Texture2D>(*t);
}

Uniform Material::get_uniform() const
{
    Uniform u = Uniform(ambient, diffuse, specular, spec_rank);
    u.base_texture = base_texture.get();
    return u;
}

Shader *Material::get_shader()
{
    return shader;
}

void Material::destroy(){}

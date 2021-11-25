//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_SHADER_HPP
#define RENDERER_SHADER_HPP

#include "Fragment.hpp"
#include "Vertex.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "State.hpp"

class Light;

class Uniform
{
public:
    float ka; // ambient coefficient
    float kd; // diffuse coefficient
    float ks; // specular coefficient
    float spec_rank;

    Camera *camera;
    std::vector<Light *> *light_source;

    Texture2D *base_texture;
    TextureType texture_type;
    SamplerType sampler;

    Uniform(){}

    Uniform(float _ka, float _kd, float _ks, float _spec):ka(_ka), kd(_kd), ks(_ks), spec_rank(_spec),
                                                          light_source(nullptr), sampler(NORMAL){}

    ~Uniform() = default;
};

class Shader
{
private:
    static Shader *shader;
protected:
    Uniform *uniform; // store current statement
public:
    Shader():uniform(nullptr){}

    virtual ~Shader() = default;

    /**
     * Set uniform parameters.
     * @param u
     */
    void set_uniform(Uniform *u)
    {
        uniform = u;
    }

    /**
     * Set shader light source.
     * @param light
     */
    void set_light(std::vector<Light *> *light)
    {
        uniform->light_source = light;
    }

    /**
     * Set camera matrix.
     * @param M
     */
    void set_camera(Camera *c)
    {
        uniform->camera = c;
    }

    /**
     * Set texture type.
     * @param type
     */
    void set_texture_type(TextureType type)
    {
        uniform->texture_type = type;
    }

    /**
     * Set texture sampler type.
     * @param type
     */
    void set_sampler(SamplerType type)
    {
        uniform->sampler = type;
    }

    /**
    * Get a instance and reset.
    */
    static Shader *instance()
    {
        if (shader == nullptr)
            shader = new Shader();
        return shader;
    };

    virtual void vertex_shader(VertexP &vertex){}

    virtual void fragment_shader(Fragment &frag){}

    virtual void destroy()
    {
        uniform = nullptr;
        delete shader;
        shader = nullptr;
    }
};

#endif

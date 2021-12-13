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
#include "Ray.hpp"

class Light;

class Uniform
{
public:
    float ka; // ambient coefficient
    float kd; // diffuse coefficient
    float ks; // specular coefficient
    float specRank;

    Camera *camera;
    std::vector<Light *> *lightSource;

    Texture2D *baseTexture;
    Render::TextureType textureType;
    Render::SamplerType samplerType;

    Uniform(){}

    Uniform(float _ka, float _kd, float _ks, float _spec):ka(_ka), kd(_kd), ks(_ks), specRank(_spec),
                                                          lightSource(nullptr), samplerType(Render::NORMAL){}

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
    void setUniform(Uniform *u)
    {
        uniform = u;
    }

    /**
     * Set shader light source.
     * @param light
     */
    void setLight(std::vector<Light *> *light)
    {
        uniform->lightSource = light;
    }

    /**
     * Set camera matrix.
     * @param M
     */
    void setCamera(Camera *c)
    {
        uniform->camera = c;
    }

    /**
     * Set texture type.
     * @param type
     */
    void setTextureType(Render::TextureType type)
    {
        uniform->textureType = type;
    }

    /**
     * Set texture sampler type.
     * @param type
     */
    void setSampler(Render::SamplerType type)
    {
        uniform->samplerType = type;
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

    /**
     *
     * @param vertex
     */
    virtual void vertexShader(VertexP &vertex){}

    /**
     *
     * @param frag
     * @return color
     */
    virtual float4 fragmentShader(Fragment &frag)
    {
        return {0, 0, 0, 1.0f};
    }

    /**
     *
     * @param record
     * @return color
     */
    virtual float4 rayShader(HitRecord &record)
    {
        return {0, 0, 0, 1.0f};
    }

    /**
     *
     */
    virtual void destroy()
    {
        uniform = nullptr;
        delete shader;
        shader = nullptr;
    }
};

#endif

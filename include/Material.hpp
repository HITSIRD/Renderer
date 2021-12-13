//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_MATERIAL_HPP
#define RENDERER_MATERIAL_HPP

#include "Shader.hpp"
#include "Texture.hpp"

class Material
{
protected:
    Shader *shader;

public:
    std::shared_ptr<Texture2D> baseTexture;
    std::shared_ptr<Texture2D> normalTexture;
//    std::shared_ptr<Texture> glossness_texture;
//    std::shared_ptr<Texture> metalness_texture;
//    std::shared_ptr<Texture> base_texture;
//    std::shared_ptr<Texture> base_texture;
    float ambient;
    float diffuse;
    float specular;
    float specRank;

    Material();

    virtual ~Material();

    /**
     *
     * @param s
     */
    void setShader(Shader *s);

    /**
     *
     * @param t
     */
    void setTexture(Texture2D *t);

    /**
     *
     * @param u
     */
    Uniform getUniform() const;

    /**
     *
     * @return shader
     */
    Shader *getShader();

    virtual void destroy();
};

#endif //RENDERER_MATERIAL_HPP

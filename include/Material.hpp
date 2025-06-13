//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_MATERIAL_HPP
#define RENDERER_MATERIAL_HPP

#include "Shader.hpp"
#include "Texture.hpp"

namespace Renderer {
    enum TextureType {
        TEXTURE_BASE, TEXTURE_NORMAL, TEXTURE_AO, TEXTURE_METALNESS, TEXTURE_ROUGHNESS, TEXTURE_EMISSION,
    };
}

class Material {
protected:
    Shader *shader;

public:
    std::shared_ptr<Texture2D<unsigned char> > textureBase; // base color texture
    std::shared_ptr<Texture2D<unsigned char> > textureNormal; // normal texture
    std::shared_ptr<Texture2D<unsigned char> > textureAO; // ambient occlusion texture
    std::shared_ptr<Texture2D<unsigned char> > textureMetalness; // metalness texture
    std::shared_ptr<Texture2D<unsigned char> > textureRoughness; // roughness texture
    std::shared_ptr<Texture2D<unsigned char> > textureEmission; // emission texture

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
     * @param texture2D
     * @param type
     */
    void setTexture(Texture2D<unsigned char> *texture2D, Renderer::TextureType type);

    /**
     *
     * @param uniform
     */
    virtual void setUniform(Uniform &uniform) const;

    /**
     *
     * @return shader
     */
    Shader *getShader();

    virtual void destroy();
};

#endif //RENDERER_MATERIAL_HPP

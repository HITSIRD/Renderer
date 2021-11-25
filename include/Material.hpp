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
    std::shared_ptr<Texture2D> base_texture;
    std::shared_ptr<Texture2D> normal_texture;
//    std::shared_ptr<Texture> base_texture;
//    std::shared_ptr<Texture> base_texture;
//    std::shared_ptr<Texture> base_texture;
//    std::shared_ptr<Texture> base_texture;
    float ambient;
    float diffuse;
    float specular;
    float spec_rank;

    Material();

    virtual ~Material();

    /**
     *
     * @param s
     */
    void set_shader(Shader *s);

    /**
     *
     * @param t
     */
    void set_texture(Texture2D *t);

    /**
     *
     * @param u
     */
    Uniform get_uniform() const;

    /**
     *
     * @return
     */
    Shader *get_shader();

    virtual void destroy();
};

#endif //RENDERER_MATERIAL_HPP

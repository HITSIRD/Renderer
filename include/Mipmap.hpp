//
// Created by 闻永言 on 2021/10/22.
//

#ifndef RENDERER_MIPMAP_HPP
#define RENDERER_MIPMAP_HPP

#include "Texture.hpp"

class Mipmap: public Texture2D
{
public:
    Mipmap();

    /**
     *
     * @param texture_file
     */
    Mipmap(const std::string &texture_file);

    /**
     *
     * @param texture
     */
    void initialize(Texture2D *texture);

    ~Mipmap();

    /**
     *
     * @param texture_uv
     * @param texture_x
     * @param texture_y
     * @param type
     * @return
     */
    float4 sample(const float2 &texture_uv, const float2 &texture_x, const float2 &texture_y, SamplerType type) const;
private:
    int max_level;
    int max_size; // level 0 texture size

    std::vector<Image *> data;

    /**
     *
     * @param x
     * @param y
     * @param level
     * @return
     */
    float4 sample_normal(int x, int y, int level) const;

    /**
     *
     * @param texture_uv
     * @param level
     * @return
     */
    float4 sample_normal(const float2 &texture_uv, int level) const;

    /**
     *
     * @param texture_uv
     * @param level
     * @return
     */
    float4 sample_bilinear(const float2 &texture_uv, int level) const;

    /**
     *
     * @param texture_uv
     * @param level
     * @param low
     * @return
     */
    float4 sample_trilinear(const float2 &texture_uv, float level, int low) const;

    /**
     *
     * @param texture_uv
     * @param level
     * @param low
     * @param high
     * @return
     */
    float4 sample_anisotropic(const float2 &texture_uv, float level, int low, int high) const;
};

#endif //RENDERER_MIPMAP_HPP

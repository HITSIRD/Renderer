//
// Created by 闻永言 on 2021/10/22.
//

#ifndef RENDERER_MIPMAP_HPP
#define RENDERER_MIPMAP_HPP

#include "Texture.hpp"

class Mipmap:public Texture2D
{
public:
    Mipmap();

    /**
     *
     * @param textureFile
     */
    Mipmap(const std::string &textureFile);

    /**
     *
     * @param texture
     */
    void initialize(Texture2D *texture);

    ~Mipmap();

    /**
     *
     * @param textureCoord
     * @param ddx
     * @param ddy
     * @param samplerType
     * @return
     */
    float4
    sample(const float2 &textureCoord, const float2 &ddx, const float2 &ddy, Render::SamplerType samplerType) const;

private:
    int maxLevel;
    int maxSize; // level 0 texture size

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
     * @param textureCoord
     * @param level
     * @return
     */
    float4 sample_normal(const float2 &textureCoord, int level) const;

    /**
     *
     * @param textureCoord
     * @param level
     * @return
     */
    float4 sample_bilinear(const float2 &textureCoord, int level) const;

    /**
     *
     * @param textureCoord
     * @param level
     * @param low
     * @return
     */
    float4 sample_trilinear(const float2 &textureCoord, float level, int low) const;

    /**
     *
     * @param textureCoord
     * @param level
     * @param low
     * @param high
     * @return
     */
    float4 sample_anisotropic(const float2 &textureCoord, float level, int low, int high) const;
};

#endif //RENDERER_MIPMAP_HPP

//
// Created by 闻永言 on 2021/9/11.
//

#ifndef RENDERER_TEXTURE_HPP
#define RENDERER_TEXTURE_HPP

#include "Type.hpp"
#include <vector>
#include <string>

class Mipmap;

namespace Render
{
    enum SamplerType
    {
        NORMAL, BILINEAR, TRILINEAR, ANISOTROPIC
    };
}

class Image
{
public:
    int x; // width of image
    int y; // height of image
    int channel;

    unsigned char *data;

    /**
     *
     * @param _x width
     * @param _y height
     * @param channel
     */
    Image(int _x, int _y, int _channel);

    ~Image();

    /**
     *
     * @param d
     */
    void setData(const unsigned char *d);

    /**
     *
     * @return
     */
    unsigned char *getData() const;
};

class Texture2D
{
public:
    Texture2D();

    /**
     *
     * @param texture_file
     */
    Texture2D(const std::string &texture_file);

    ~Texture2D();

    /**
     *
     * @oaram texture_uv
     * @param type
     * @return
     */
    float4 sample(const float2 &texture_uv, Render::SamplerType type) const;

    /**
     *
     * @param textureCoord
     * @param ddx
     * @param ddy
     * @param samplerType
     * @return
     */
    float4 sample(const float2 &textureCoord, const float2 &ddx, const float2 &ddy, Render::SamplerType samplerType) const;

    /**
     * Generate mipmap of texture.
     */
    void initializeMipmap();

    /**
     *
     * @return size
     */
    int getSize() const;

    /**
     *
     * @return channel
     */
    int getChannel() const;

    /**
     *
     * @return
     */
    unsigned char *getData() const;

private:
    int size;
    Image *image;
    Mipmap *mipmap;

    /**
     *
     * @param x
     * @param y
     * @return
     */
    float4 sampleNormal(int x, int y) const;

    /**
     *
     * @param texture_uv
     * @return
     */
    float4 sampleNormal(const float2 &texture_uv) const;

    /**
     *
     * @param texture_uv
     * @return
     */
    float4 sampleBilinear(const float2 &texture_uv) const;
};

class Texture3D
{
    /**
     *
     * @param textureFile
     */
    Texture3D(const std::string &textureFile);


private:
//    Texture2D top;
//    Texture2D bottom;
//    Texture2D left;
//    Texture2D right;
//    Texture2D front;
//    Texture2D back;
};

#endif //RENDERER_TEXTURE_HPP

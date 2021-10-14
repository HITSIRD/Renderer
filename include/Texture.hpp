//
// Created by 闻永言 on 2021/9/11.
//

#ifndef RENDERER_TEXTURE_HPP
#define RENDERER_TEXTURE_HPP

#include "Type.hpp"
#include <string>

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
    void set_data(const unsigned char *d);
};

class Texture
{
public:
    int x; // width of image
    int y; // height of image
    Image *image;

    /**
     *
     * @param texture_file
     */
    Texture(const std::string &texture_file);

    ~Texture();

    /**
     *
     * @oaram texture_uv
     * @return
     */
    float4 sample(float2 texture_uv) const;

    float4 sample_bilinear(float2 texture_uv) const;
};

#endif //RENDERER_TEXTURE_HPP

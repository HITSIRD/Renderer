//
// Created by 闻永言 on 2021/9/11.
//

#ifndef RENDERER_TEXTURE_HPP
#define RENDERER_TEXTURE_HPP

#include "Type.hpp"
#include <vector>
#include <string>

class Mipmap;

enum SamplerType
{
    NORMAL, BILINEAR, TRILINEAR, ANISOTROPIC
};

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

    /**
     *
     * @return
     */
    unsigned char *get_data() const;
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
    float4 sample(const float2 &texture_uv, SamplerType type) const;

    /**
     *
     * @param texture_uv
     * @param texture_x
     * @param texture_y
     * @param type
     * @return
     */
    float4 sample(const float2 &texture_uv, const float2 &texture_x, const float2 &texture_y, SamplerType type) const;

    /**
     * Generate mipmap of texture.
     */
    void initialize_mipmap();

    /**
     *
     * @return size
     */
    int get_size() const;

    /**
     *
     * @return channel
     */
    int get_channel() const;

    /**
     *
     * @return
     */
    unsigned char *get_data() const;

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
    float4 sample_normal(int x, int y) const;

    /**
     *
     * @param texture_uv
     * @return
     */
    float4 sample_normal(const float2 &texture_uv) const;

    /**
     *
     * @param texture_uv
     * @return
     */
    float4 sample_bilinear(const float2 &texture_uv) const;
};

class Texture3D
{
    /**
     *
     * @param texture_file
     */
    Texture3D(const std::string &texture_file);


private:
    Texture2D top;
    Texture2D bottom;
    Texture2D left;
    Texture2D right;
    Texture2D front;
    Texture2D back;
};

#endif //RENDERER_TEXTURE_HPP

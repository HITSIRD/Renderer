//
// Created by 闻永言 on 2021/10/22.
//

#include "Mipmap.hpp"
#include "Util.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace std;

#define log2 0.6931471806f

class Texture2D;

Mipmap::Mipmap() = default;

Mipmap::Mipmap(const std::string &texture_file)
{
    cv::Mat img = cv::imread(texture_file, cv::ImreadModes::IMREAD_UNCHANGED);
    assert(img.rows == img.cols && ((img.rows & (img.rows - 1)) == 0)); // size must be 2^N

    max_level = 0;
    int size = img.rows;
    max_size = size;
    int channels = img.channels();
    while (size != 1)
    {
        max_level++;
        size = size >> 1;
    }

    // add level 0
    auto *image_0 = new Image(max_size, max_size, img.channels());
    image_0->set_data(img.data);
    data.push_back(image_0);

    size = img.rows;
    for (int i = 0; i < max_level; i++)
    {
        size = size >> 1;
        auto *image = new Image(size, size, channels);
        if (!image->data)
        {
            image->data = new uchar[size * size * channels];
        }

        data.push_back(image);
        for (int row = 0; row < size; row++)
        {
            for (int col = 0; col < size; col++)
            {
                int offset = (row * size + col) * channels;
                int up_offset = (4 * row * size + 2 * col) * channels;
                for (int c = 0; c < channels; c++)
                {
                    unsigned char d_0 = data[i]->data[up_offset + c];
                    unsigned char d_1 = data[i]->data[up_offset + c + channels];
                    unsigned char d_2 = data[i]->data[up_offset + c + 4 * size];
                    unsigned char d_3 = data[i]->data[up_offset + c + 4 * size + channels];
                    image->data[offset + c] = (d_0 + d_1 + d_2 + d_3) / 4;
                }
            }
        }
    }
}

void Mipmap::initialize(Texture2D *texture)
{
    max_level = 0;
    int size = texture->get_size();
    max_size = size;
    int channels = texture->get_channel();

    while (size != 1)
    {
        max_level++;
        size = size >> 1;
    }

    // add level 0
    auto *image_0 = new Image(max_size, max_size, channels);
    image_0->set_data(texture->get_data());
    data.push_back(image_0);

    size = max_size;
    for (int i = 0; i < max_level; i++)
    {
        size = size >> 1;
        auto *image = new Image(size, size, channels);
        if (!image->data)
        {
            image->data = new uchar[size * size * channels];
        }

        data.push_back(image);
        for (int row = 0; row < size; row++)
        {
            for (int col = 0; col < size; col++)
            {
                int offset = (row * size + col) * channels;
                int up_offset = (4 * row * size + 2 * col) * channels;
                for (int c = 0; c < channels; c++)
                {
                    unsigned char d_0 = data[i]->data[up_offset + c];
                    unsigned char d_1 = data[i]->data[up_offset + c + channels];
                    unsigned char d_2 = data[i]->data[up_offset + c + 2 * size * channels];
                    unsigned char d_3 = data[i]->data[up_offset + c + 2 * size * channels + channels];
                    image->data[offset + c] = (d_0 + d_1 + d_2 + d_3) / 4;
                }
            }
        }
    }
}

Mipmap::~Mipmap()
{
    for (auto p: data)
    {
        delete[] p;
    }
}

float4 Mipmap::sample(
        const float2 &texture_uv, const float2 &texture_x, const float2 &texture_y, SamplerType type) const
{
    float ux = (texture_x - texture_uv).lpNorm<2>();
    float uy = (texture_y - texture_uv).lpNorm<2>();
    float level = log(fmax(ux, uy) * (float)max_size) / log2;
    level = level > (float)max_level ? (float)max_level - 0.001f : level;
    int high = (int)(level) + 1;
    int low = (int)(log(fmin(ux, uy) * (float)max_size) / log2);
    if(level < 0)
    {
        level = 0;
        high = 0;
    }
    low = low < 0 ? 0 : low;

    switch (type)
    {
        case NORMAL:
            return sample_normal(texture_uv, low);
        case BILINEAR:
            return sample_bilinear(texture_uv, low);
        case TRILINEAR:
            return sample_trilinear(texture_uv, level, low);
        case ANISOTROPIC:
            return sample_anisotropic(texture_uv, level, low, high);
        default:
            return {};
    }
}

float4 Mipmap::sample_normal(int x, int y, int level) const
{
    level = level < 0 ? 0 : level;
    level = level > max_level ? max_level : level;
    int size = max_size >> level;
    x = x % size;
    y = y % size;
    x = x < 0 ? size + x : x;
    y = y < 0 ? size + y : y;
    int index = data[level]->channel * (y * size + x);
    switch (data[level]->channel)
    {
        case 1:
        {
            return {(float)data[level]->data[index] / 255.0f, 1.0f, 1.0f, 1.0f};
        }
        case 3:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / 255.0f, (float)data[level]->data[index + 1] / 255.0f,
                    (float)data[level]->data[index] / 255.0f, 1.0f};
        }
        case 4:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / 255.0f, (float)data[level]->data[index + 1] / 255.0f,
                    (float)data[level]->data[index] / 255.0f, (float)data[level]->data[index + 3] / 255.0f};
        }
    }
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

float4 Mipmap::sample_normal(const float2 &texture_uv, int level) const
{
    level = level < 0 ? 0 : level;
    level = level > max_level ? max_level : level;
    int size = max_size >> level;
    int x = (int)(texture_uv.x() * (float)size - 0.5f) % size;
    int y = (int)(texture_uv.y() * (float)size - 0.5f) % size;
    x = x < 0 ? size + x : x;
    y = y < 0 ? size + y : y;
    int index = data[level]->channel * (y * size + x);
    switch (data[level]->channel)
    {
        case 1:
        {
            return {(float)data[level]->data[index] / 255.0f, 1.0f, 1.0f, 1.0f};
        }
        case 3:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / 255.0f, (float)data[level]->data[index + 1] / 255.0f,
                    (float)data[level]->data[index] / 255.0f, 1.0f};
        }
        case 4:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / 255.0f, (float)data[level]->data[index + 1] / 255.0f,
                    (float)data[level]->data[index] / 255.0f, (float)data[level]->data[index + 3] / 255.0f};
        }
    }
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

float4 Mipmap::sample_bilinear(const float2 &texture_uv, int level) const
{
    level = level < 0 ? 0 : level;
    level = level > max_level ? max_level : level;
    int size = max_size >> level;
    float u_p = texture_uv.x() * (float)size - 0.5f;
    float v_p = texture_uv.y() * (float)size - 0.5f;
    float iu0 = floor(u_p);
    float iv0 = floor(v_p);
    float iu1 = iu0 + 1.0f;
    float iv1 = iv0 + 1.0f;

    float4 color_0 = sample_normal((int)iu0, (int)iv1, level);
    float4 color_1 = sample_normal((int)iu1, (int)iv1, level);
    float4 color_2 = sample_normal((int)iu0, (int)iv0, level);
    float4 color_3 = sample_normal((int)iu1, (int)iv0, level);
    float ratio_u = iu1 - u_p;
    float ratio_v = iv1 - v_p;
    float4 color_x0 = lerp(ratio_u, color_1, color_0);
    float4 color_x1 = lerp(ratio_u, color_3, color_2);
    return lerp(ratio_v, color_x0, color_x1);
}

float4 Mipmap::sample_trilinear(const float2 &texture_uv, float level, int low) const
{
    if(level < 0.000001f)
    {
        return sample_bilinear(texture_uv, low);
    }
    float4 low_color = sample_bilinear(texture_uv, low);
    float4 high_color = sample_bilinear(texture_uv, low + 1);
    return lerp(level - (float)low, low_color, high_color);
}

float4 Mipmap::sample_anisotropic(const float2 &texture_uv, float level, int low, int high) const
{
    int size = max_size >> (int)level;
    float u_p = texture_uv.x() * (float)size - 0.5f;
    float v_p = texture_uv.y() * (float)size - 0.5f;
    float iu0 = floor(u_p);
    float iv0 = floor(v_p);
    float iu1 = iu0 + 1.0f;
    float iv1 = iv0 + 1.0f;

    float4 color_0 = sample_normal((int)iu0, (int)iv1, level);
    float4 color_1 = sample_normal((int)iu1, (int)iv1, level);
    float4 color_2 = sample_normal((int)iu0, (int)iv0, level);
    float4 color_3 = sample_normal((int)iu1, (int)iv0, level);
    float ratio_u = iu1 - u_p;
    float ratio_v = iv1 - v_p;
    float4 color_x0 = lerp(ratio_u, color_1, color_0);
    float4 color_x1 = lerp(ratio_u, color_3, color_2);
    return lerp(ratio_v, color_x0, color_x1);
}

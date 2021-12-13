//
// Created by 闻永言 on 2021/10/22.
//

#include "Mipmap.hpp"
#include "Util.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace std;
using namespace Render;

class Texture2D;

Mipmap::Mipmap() = default;

Mipmap::Mipmap(const std::string &textureFile)
{
    cv::Mat img = cv::imread(textureFile, cv::ImreadModes::IMREAD_UNCHANGED);
    assert(img.rows == img.cols && ((img.rows & (img.rows - 1)) == 0)); // size must be 2^N

    maxLevel = 0;
    int size = img.rows;
    maxSize = size;
    int channels = img.channels();
    while (size != 1)
    {
        maxLevel++;
        size = size >> 1;
    }

    // add level 0
    auto *image_0 = new Image(maxSize, maxSize, img.channels());
    image_0->setData(img.data);
    data.push_back(image_0);

    size = img.rows;
    for (int i = 0; i < maxLevel; i++)
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
    maxLevel = 0;
    int size = texture->getSize();
    maxSize = size;
    int channels = texture->getChannel();

    while (size != 1)
    {
        maxLevel++;
        size = size >> 1;
    }

    // add level 0
    auto *image_0 = new Image(maxSize, maxSize, channels);
    image_0->setData(texture->getData());
    data.push_back(image_0);

    size = maxSize;
    for (int i = 0; i < maxLevel; i++)
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
        const float2 &textureCoord, const float2 &ddx, const float2 &ddy, SamplerType samplerType) const
{
    float ux = (ddx - textureCoord).lpNorm<2>();
    float uy = (ddy - textureCoord).lpNorm<2>();
    float level = log(fmax(ux, uy) * (float)maxSize) * InvLog2;
    level = level > (float)maxLevel ? (float)maxLevel - 0.001f : level;
    int high = (int)(level) + 1;
    int low = (int)(log(fmin(ux, uy) * (float)maxSize) * InvLog2);
    if (level < 0)
    {
        level = 0;
        high = 0;
    }
    low = low < 0 ? 0 : low;

    switch (samplerType)
    {
        case NORMAL:
            return sample_normal(textureCoord, low);
        case BILINEAR:
            return sample_bilinear(textureCoord, low);
        case TRILINEAR:
            return sample_trilinear(textureCoord, level, low);
        case ANISOTROPIC:
            return sample_anisotropic(textureCoord, level, low, high);
        default:
            return {};
    }
}

float4 Mipmap::sample_normal(int x, int y, int level) const
{
    level = level < 0 ? 0 : level;
    level = level > maxLevel ? maxLevel : level;
    int size = maxSize >> level;
    x = x % size;
    y = y % size;
    x = x < 0 ? size + x : x;
    y = y < 0 ? size + y : y;
    int index = data[level]->channel * (y * size + x);
    switch (data[level]->channel)
    {
        case 1:
        {
            return {(float)data[level]->data[index] / Inv255, 1.0f, 1.0f, 1.0f};
        }
        case 3:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / Inv255, (float)data[level]->data[index + 1] / Inv255,
                    (float)data[level]->data[index] / Inv255, 1.0f};
        }
        case 4:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / Inv255, (float)data[level]->data[index + 1] / Inv255,
                    (float)data[level]->data[index] / Inv255, (float)data[level]->data[index + 3] / Inv255};
        }
    }
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

float4 Mipmap::sample_normal(const float2 &textureCoord, int level) const
{
    level = level < 0 ? 0 : level;
    level = level > maxLevel ? maxLevel : level;
    int size = maxSize >> level;
    int x = (int)(textureCoord.x() * (float)size - 0.5f) % size;
    int y = (int)(textureCoord.y() * (float)size - 0.5f) % size;
    x = x < 0 ? size + x : x;
    y = y < 0 ? size + y : y;
    int index = data[level]->channel * (y * size + x);
    switch (data[level]->channel)
    {
        case 1:
        {
            return {(float)data[level]->data[index] / Inv255, 1.0f, 1.0f, 1.0f};
        }
        case 3:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / Inv255, (float)data[level]->data[index + 1] / Inv255,
                    (float)data[level]->data[index] / Inv255, 1.0f};
        }
        case 4:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)data[level]->data[index + 2] / Inv255, (float)data[level]->data[index + 1] / Inv255,
                    (float)data[level]->data[index] / Inv255, (float)data[level]->data[index + 3] / Inv255};
        }
    }
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

float4 Mipmap::sample_bilinear(const float2 &textureCoord, int level) const
{
    level = level < 0 ? 0 : level;
    level = level > maxLevel ? maxLevel : level;
    int size = maxSize >> level;
    float u_p = textureCoord.x() * (float)size - 0.5f;
    float v_p = textureCoord.y() * (float)size - 0.5f;
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

float4 Mipmap::sample_trilinear(const float2 &textureCoord, float level, int low) const
{
    if (level < 0.000001f)
    {
        return sample_bilinear(textureCoord, low);
    }
    float4 low_color = sample_bilinear(textureCoord, low);
    float4 high_color = sample_bilinear(textureCoord, low + 1);
    return lerp(level - (float)low, low_color, high_color);
}

float4 Mipmap::sample_anisotropic(const float2 &textureCoord, float level, int low, int high) const
{
    int size = maxSize >> (int)level;
    float u_p = textureCoord.x() * (float)size - 0.5f;
    float v_p = textureCoord.y() * (float)size - 0.5f;
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

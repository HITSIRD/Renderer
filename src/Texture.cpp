//
// Created by 闻永言 on 2021/9/13.
//

#include "Texture.hpp"
#include "Mipmap.hpp"
#include "Util.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace Render;

Image::Image(int _x, int _y, int _channel)
{
    x = _x;
    y = _y;
    channel = _channel;
    data = nullptr;
}

Image::~Image()
{
    delete[] data;
    data = nullptr;
}

void Image::setData(const uchar *d)
{
    if (data == nullptr)
    {
        data = new uchar[x * y * channel];
    }
    for (int i = 0; i < x * y * channel; i++)
    {
        data[i] = d[i];
    }
}

unsigned char *Image::getData() const
{
    return data;
}

Texture2D::Texture2D() = default;

Texture2D::Texture2D(const string &texture_file):mipmap(nullptr)
{
    cv::Mat img = cv::imread(texture_file, cv::ImreadModes::IMREAD_UNCHANGED);
    assert(img.rows == img.cols && ((img.rows & (img.rows - 1)) == 0)); // size must be 2^N
    size = img.cols;
    image = new Image(img.cols, img.rows, img.channels());
    image->setData(img.data);
}

Texture2D::~Texture2D()
{
    delete image;
    delete mipmap;
}

int Texture2D::getSize() const
{
    return size;
}

int Texture2D::getChannel() const
{
    return image->channel;
}

unsigned char *Texture2D::getData() const
{
    return image->getData();
}

float4 Texture2D::sample(const float2 &texture_uv, SamplerType type = NORMAL) const
{
    switch (type)
    {
        case NORMAL:
            return sampleNormal(texture_uv);
        case BILINEAR:
            return sampleBilinear(texture_uv);
        default:
            return {};
    }
}

float4 Texture2D::sample(
        const float2 &textureCoord, const float2 &ddx, const float2 &ddy, SamplerType samplerType) const
{
    if (mipmap)
    {
        return mipmap->sample(textureCoord, ddx, ddy, samplerType);
    }
    // error
    return {};
}

void Texture2D::initializeMipmap()
{
    if (!mipmap)
    {
        mipmap = new Mipmap();
        mipmap->initialize(this);
        delete image; // to save memory
    }
}

float4 Texture2D::sampleNormal(int x, int y) const
{
    x = x % size;
    y = y % size;
    x = x < 0 ? size + x : x;
    y = y < 0 ? size + y : y;
    int index = image->channel * (y * size + x);
    switch (image->channel)
    {
        case 1:
        {
            return {(float)image->data[index] / 255.0f, 1.0f, 1.0f, 1.0f};
        }
        case 3:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)image->data[index + 2] / 255.0f, (float)image->data[index + 1] / 255.0f,
                    (float)image->data[index] / 255.0f, 1.0f};
        }
        case 4:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)image->data[index + 2] / 255.0f, (float)image->data[index + 1] / 255.0f,
                    (float)image->data[index] / 255.0f, (float)image->data[index + 3] / 255.0f};
        }
    }
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

float4 Texture2D::sampleNormal(const float2 &texture_uv) const
{
    int x = (int)(texture_uv.x() * (float)size - 0.5f) % size;
    int y = (int)(texture_uv.y() * (float)size - 0.5f) % size;
    x = x < 0 ? size + x : x;
    y = y < 0 ? size + y : y;
    int index = image->channel * (y * size + x);
    switch (image->channel)
    {
        case 1:
        {
            return {(float)image->data[index] / 255.0f, 1.0f, 1.0f, 1.0f};
        }
        case 3:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)image->data[index + 2] / 255.0f, (float)image->data[index + 1] / 255.0f,
                    (float)image->data[index] / 255.0f, 1.0f};
        }
        case 4:
        {
            // OpenCV color channel: GBR -> RGB
            return {(float)image->data[index + 2] / 255.0f, (float)image->data[index + 1] / 255.0f,
                    (float)image->data[index] / 255.0f, (float)image->data[index + 3] / 255.0f};
        }
    }
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

float4 Texture2D::sampleBilinear(const float2 &texture_uv) const
{
    float u_p = texture_uv.x() * (float)size - 0.5f;
    float v_p = texture_uv.y() * (float)size - 0.5f;
    float iu0 = floor(u_p);
    float iv0 = floor(v_p);
    float iu1 = iu0 + 1.0f;
    float iv1 = iv0 + 1.0f;

    float4 color_0 = sampleNormal((int)iu0, (int)iv1);
    float4 color_1 = sampleNormal((int)iu1, (int)iv1);
    float4 color_2 = sampleNormal((int)iu0, (int)iv0);
    float4 color_3 = sampleNormal((int)iu1, (int)iv0);
    float ratio_u = iu1 - u_p;
    float ratio_v = iv1 - v_p;
    float4 color_x0 = lerp(ratio_u, color_1, color_0);
    float4 color_x1 = lerp(ratio_u, color_3, color_2);
    return lerp(ratio_v, color_x0, color_x1);
}

//
// Created by 闻永言 on 2021/9/13.
//

#include "Texture.hpp"
#include "Mipmap.hpp"
#include "Util.hpp"
#include "opencv2/highgui.hpp"

using namespace std;

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

void Image::set_data(const uchar *d)
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

unsigned char *Image::get_data() const
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
    image->set_data(img.data);
}

Texture2D::~Texture2D()
{
    delete image;
    delete mipmap;
}

int Texture2D::get_size() const
{
    return size;
}

int Texture2D::get_channel() const
{
    return image->channel;
}

unsigned char *Texture2D::get_data() const
{
    return image->get_data();
}

float4 Texture2D::sample(const float2 &texture_uv, SamplerType type = NORMAL) const
{
    switch (type)
    {
        case NORMAL:
            return sample_normal(texture_uv);
        case BILINEAR:
            return sample_bilinear(texture_uv);
        default:
            return {};
    }
}

float4 Texture2D::sample(
        const float2 &texture_uv, const float2 &texture_x, const float2 &texture_y, SamplerType type) const
{
    if (mipmap)
    {
        return mipmap->sample(texture_uv, texture_x, texture_y, type);
    }
    // error
    return {};
}

void Texture2D::initialize_mipmap()
{
    if (!mipmap)
    {
        mipmap = new Mipmap();
        mipmap->initialize(this);
        delete image; // to save memory
    }
}

float4 Texture2D::sample_normal(int x, int y) const
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

float4 Texture2D::sample_normal(const float2 &texture_uv) const
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

float4 Texture2D::sample_bilinear(const float2 &texture_uv) const
{
    float u_p = texture_uv.x() * (float)size - 0.5f;
    float v_p = texture_uv.y() * (float)size - 0.5f;
    float iu0 = floor(u_p);
    float iv0 = floor(v_p);
    float iu1 = iu0 + 1.0f;
    float iv1 = iv0 + 1.0f;

    float4 color_0 = sample_normal((int)iu0, (int)iv1);
    float4 color_1 = sample_normal((int)iu1, (int)iv1);
    float4 color_2 = sample_normal((int)iu0, (int)iv0);
    float4 color_3 = sample_normal((int)iu1, (int)iv0);
    float ratio_u = iu1 - u_p;
    float ratio_v = iv1 - v_p;
    float4 color_x0 = lerp(ratio_u, color_1, color_0);
    float4 color_x1 = lerp(ratio_u, color_3, color_2);
    return lerp(ratio_v, color_x0, color_x1);
}

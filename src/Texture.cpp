//
// Created by 闻永言 on 2021/9/13.
//

#include "Texture.hpp"
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
    data = new uchar[x * y * channel];
    for (int i = 0; i < x * y * channel; i++)
    {
        data[i] = d[i];
    }
}

Texture::Texture(const string &texture_file)
{
    cv::Mat img = cv::imread(texture_file, cv::ImreadModes::IMREAD_UNCHANGED);
    x = img.cols;
    y = img.rows;
    image = new Image(x, y, img.channels());
    image->set_data(img.data);
}

Texture::~Texture()
{
    image->~Image();
}

float4 Texture::sample(float2 texture_uv) const
{
    int index_x = (int)(texture_uv.x() * (float)x - 0.5f) % x;
    int index_y = (int)(texture_uv.y() * (float)y - 0.5f) % y;
    index_x = index_x < 0 ? x + index_x : index_x;
    index_y = index_y < 0 ? y + index_y : index_y;
    int index = image->channel * (index_y * x + index_x);
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

float4 Texture::sample_bilinear(float2 texture_uv) const
{
    float4 color(0, 0, 0, 0);
    for (int j = 0; j <= 1; j++)
    {
        for (int i = 0; i <= 1; i++)
        {
            float offset = 1.0f / (float)x;
            float2 uv(texture_uv.x() + offset * (float)i, texture_uv.y() + offset * (float)j);
            color += sample(uv);
        }
    }
    return color / 4.0f;
}


//
// Created by 闻永言 on 2021/8/28.
//

#include "State.hpp"

void FrameBuffer::reset() const
{
    if (buffer)
    {
        int size = 4 * x * y;
        for (int i = 0; i < size; i += 4)
        {
            buffer[i] = 0;
            buffer[i + 1] = 0;
            buffer[i + 2] = 0;
            buffer[i + 3] = 255;
        }
    }
}

FrameBuffer::FrameBuffer(int _x, int _y):x(_x), y(_y)
{
    buffer = new unsigned char[4 * x * y];
}

FrameBuffer::~FrameBuffer()
{
    if (buffer)
    {
        delete[] buffer;
    }
}

void FrameBuffer::write_color(const Fragment &frag) const
{
    int r = (int)(255.0f * (frag.color.x() > 1.0f ? 1.0f : frag.color.x()));
    int g = (int)(255.0f * (frag.color.y() > 1.0f ? 1.0f : frag.color.y()));
    int b = (int)(255.0f * (frag.color.z() > 1.0f ? 1.0f : frag.color.z()));
    int a = 255;
    int index = 4 * (frag.y * x + frag.x);
    buffer[index] = r;
    buffer[index + 1] = g;
    buffer[index + 2] = b;
    buffer[index + 3] = a;
}

State::~State()
{
    if (model)
    {
        delete model;
    }
    if (camera)
    {
        delete camera;
    }
    if (shader)
    {
        delete shader;
    }
    for (auto l: light)
    {
        delete l;
    }
    if (stencil_buffer)
    {
        delete[] stencil_buffer;
    }
    if (z_buffer)
    {
        delete[] z_buffer;
    }
    if (color_buffer)
    {
        delete[] color_buffer;
    }
    if (frame_buffer)
    {
        delete frame_buffer;
    }
}

void State::reset_buffer() const
{
    int size = camera->x * camera->y;
//    if (stencil_buffer)
//    {
//        for (int i = 0; i < size; i++)
//        {
//            stencil_buffer[i] = 0;
//        }
//    }

    if (z_buffer)
    {
        for (int i = 0; i < size; i++)
        {
            z_buffer[i] = 1.0f;
        }
    }

//    if (color_buffer)
//    {
//        for (int i = 0; i < size; i++)
//        {
//            color_buffer[i] << 0, 0, 0, 1.0f;
//        }
//    }

    frame_buffer->reset();
}

void State::destroy() const
{
    delete model;
    delete camera;
    delete stencil_buffer;
    delete z_buffer;
    delete color_buffer;
    delete frame_buffer;
    for (auto &l: light)
    {
        delete l;
    }
}
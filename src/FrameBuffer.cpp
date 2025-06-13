//
// Created by 闻永言 on 2021/12/22.
//

#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer(): colorBuffer(nullptr) {
}

FrameBuffer::FrameBuffer(int x, int y): colorBuffer(new Image<float>(x, y, 4)) {
}

FrameBuffer::~FrameBuffer() {
    delete colorBuffer;
}

void FrameBuffer::writeColor(int index, const float4 &color) const {
    index *= 4;
    colorBuffer->data[index] = color.x() > 1.0f ? 1.0f : color.x();
    colorBuffer->data[index + 1] = color.y() > 1.0f ? 1.0f : color.y();
    colorBuffer->data[index + 2] = color.z() > 1.0f ? 1.0f : color.z();
    colorBuffer->data[index + 3] = 1.0f;
}

void FrameBuffer::writeColor(int x, int y, const float4 &color) const {
    //    int r = (int)(255.0f * (color.x() > 1.0f ? 1.0f : color.x()));
    //    int g = (int)(255.0f * (color.y() > 1.0f ? 1.0f : color.y()));
    //    int b = (int)(255.0f * (color.z() > 1.0f ? 1.0f : color.z()));
    //    int a = 255;

    int index = y * colorBuffer->x + x;
    index *= 4;
    colorBuffer->data[index] = color.x() > 1.0f ? 1.0f : color.x();
    colorBuffer->data[index + 1] = color.y() > 1.0f ? 1.0f : color.y();
    colorBuffer->data[index + 2] = color.z() > 1.0f ? 1.0f : color.z();
    colorBuffer->data[index + 3] = 1.0f;
}

void FrameBuffer::reset() const {
    if (colorBuffer) {
        int size = colorBuffer->x * colorBuffer->y;
        float *c = colorBuffer->data;
        for (int i = 0; i < size; i++) {
            *c = 0;
            c++;
            *c = 0;
            c++;
            *c = 0;
            c++;
            *c = 255;
            c++;
        }
    }
}

//
// Created by 闻永言 on 2021/12/22.
//

#ifndef RENDERER_FRAMEBUFFER_HPP
#define RENDERER_FRAMEBUFFER_HPP

#include "Type.hpp"
#include "Image.hpp"

class FrameBuffer {
public:
    Image<float> *colorBuffer;

    FrameBuffer();

    /**
     * Initialize buffer.
     * @param x
     * @param y
     */
    FrameBuffer(int x, int y);

    ~FrameBuffer();

    /**
     *
     * @param index
     * @param color
     */
    void writeColor(int index, const float4 &color) const;

    /**
     *
     * @param x
     * @param y
     * @param color
     */
    void writeColor(int x, int y, const float4 &color) const;

    /**
     *
     */
    void reset() const;
};

#endif //RENDERER_FRAMEBUFFER_HPP

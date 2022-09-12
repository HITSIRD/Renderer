//
// Created by 闻永言 on 2021/8/28.
//

#include "State.hpp"
#include <iostream>

using namespace std;
using namespace Renderer;

State::State(): screenSize({0, 0}), model(new Model()), changed(true), projection(Renderer::PERS),
        faceCullMode(Renderer::BACK), sampler(Renderer::NORMAL), mipmap(false), shadow(Renderer::SHADOW), depthTest(true),
        maxSample(16), camera(nullptr), shader(nullptr),
        //    test_buffer(nullptr),
        zBuffer(nullptr), numThreads(0)
{}

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
    for (auto l: lightSource)
    {
        delete l;
    }
//    if (stencil_buffer)
//    {
//        delete[] stencil_buffer;
//    }
    if (zBuffer)
    {
        delete zBuffer;
    }
}

void State::resetBuffer() const
{
    if (zBuffer)
    {
        zBuffer->reset(1.f);
    }
}

void State::check()
{
    if (!mipmap)
    {
        if (sampler == TRILINEAR || sampler == ANISOTROPIC)
        {
            std::cerr << "ERROR SAMPLER TYPE" << endl;
        }
    }
}

void State::destroy() const
{
    delete model;
    delete[] camera;
//    delete[] stencil_buffer;
    delete[] zBuffer;
    for (auto &l: lightSource)
    {
        delete l;
    }
}

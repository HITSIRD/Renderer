//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_STATE_HPP
#define RENDERER_STATE_HPP

#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "Model.hpp"

enum RENDER_SCENE
{
    FLAT, PHONG, PBR
};

enum FACE_CULL_MODE
{
    NONE, FRONT, BACK
};

class FrameBuffer{
public:
    int x;
    int y;
    unsigned char *buffer;

    FrameBuffer(){
        x = 0;
        y = 0;
        buffer = nullptr;
    }

    /**
     * Initialize buffer.
     * @param _x
     * @param _y
     */
    FrameBuffer(int _x, int _y);

    ~FrameBuffer();

    /**
     *
     * @param frag
     */
    void write_color(const Fragment &frag) const;

    /**
     *
     */
    void reset() const;
};

class State
{
public:
    Model *model; // meshes after clipping, update after each frame
    bool changed; // if current s is changed

//    RENDER_SCENE render_scene;
    FACE_CULL_MODE face_cull_mode;
    bool shadow; // if draw shadow

    Camera *camera;
    vector<Light*> light;

    Shader *shader;

    float *stencil_buffer;
    float *z_buffer; // depth buffer
//    bool *test_buffer; // test
    float4 *color_buffer; // store current pixel color
    FrameBuffer *frame_buffer;

    int num_threads; // max thread number

    State():
    model(new Model()),
    changed(true),
    face_cull_mode(BACK),
    shadow(false),
    camera(nullptr),
    shader(nullptr),
    stencil_buffer(nullptr),
    z_buffer(nullptr),
//    test_buffer(nullptr),
    color_buffer(nullptr),
    num_threads(0)
    {}

    ~State();

    /**
     *
     */
    void reset_buffer() const;

    void clear();

    void destroy() const;
};

#endif //RENDERER_STATE_HPP

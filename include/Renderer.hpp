//
// Created by 闻永言 on 2021/8/8.
//

#ifndef RENDERER_RENDERER_HPP
#define RENDERER_RENDERER_HPP

#include "Type.hpp"
#include "State.hpp"

namespace Render
{
    enum{
        DO_LOOP = 0,
        OUTPUT_SINGLE = 1,
        ANIMATION = 2
    };
}

class Renderer
{
public:
    State *s;
    bool stop; // if loop should stop rendering

    Renderer():stop(false), s(nullptr){}

    Renderer(bool _stop):stop(_stop), s(nullptr){}

    ~Renderer()
    {
        delete s;
    }

    /**
     *
     * @param model_file
     * @param camera_file
     */
    void read_data(const std::string &model_file, const std::string &config_file);

    /**
     *
     */
    void render(int mode);
private:
    /**
     *
     */
    void render_loop();

    /**
     *
     */
    void initialize() const;

    /**
     *
     */
    void view_transform();

    /**
     * Homogeneous space clipping.
     * @param v_0
     * @param v_1
     * @param v_2
     * @return
     */
    static bool clipping(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2);

    /**
     * Homogeneous space clipping
     * @param v_0
     * @param v_1
     * @param v_2
     * @return
     */
    static std::vector<VertexP> clip_near(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2);

    /**
     *
     */
    void draw();

    /**
     *
     * @param v_0
     * @param v_1
     * @param v_2
     * @param normal
     */
    inline void draw_triangle(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2, const float4 &normal);

    /**
     *
     */
    void alpha_test();

    /**
     *
     */
    void stencil_test();

    /**
     * Write fragment color to buffer and test depth.
     * @param frag
     */
    void write_color(Fragment &frag) const;

    /**
     *
     */
    void dither();

    /**
     * Blend to generate frame.
     */
    void frame_blend();
};

#endif //RENDERER_RENDERER_HPP

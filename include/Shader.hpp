//
// Created by 闻永言 on 2021/8/7.
//

#ifndef RENDERER_SHADER_HPP
#define RENDERER_SHADER_HPP

#include "Fragment.hpp"
#include "HitRecord.hpp"
#include "Texture.hpp"
#include "Uniform.hpp"

class Shader {
private:
    static Shader *shader;

protected:
    Uniform uniform; // store current statement
public:
    Shader() = default;

    virtual ~Shader() = default;

    /**
     * Set uniform parameters.
     * @param u
     */
    void setUniform(const Uniform &u) {
        uniform = u;
    }

    /**
     * Get a instance and reset.
     * @return shader instance
     */
    static Shader *getInstance() {
        if (!shader) {
            shader = new Shader();
        }
        return shader;
    };

    /**
     *
     * @param vertex
     */
    virtual void vertexShader(VertexP &vertex) {
    }

    /**
     *
     * @param frag
     * @return color
     */
    virtual float4 fragmentShader(Fragment &frag) {
        return {0, 0, 0, 1.0f};
    }

    /**
     *
     * @param record
     * @return color
     */
    virtual float4 rayShader(HitRecord &record) {
        return {0, 0, 0, 1.0f};
    }

    /**
     *
     */
    virtual void destroy() {
        // uniform = nullptr;
        delete shader;
        shader = nullptr;
    }
};

#endif

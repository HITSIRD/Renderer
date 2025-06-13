//
// Created by 闻永言 on 2021/12/23.
//

//
// Created by 闻永言 on 2021/12/23.
//

#include "PointLightShadowShader.hpp"
#include "Util.hpp"

using namespace std;

PointLightShadowShader *PointLightShadowShader::shader = nullptr;

PointLightShadowShader::~PointLightShadowShader() = default;

PointLightShadowShader *PointLightShadowShader::instance() {
    if (!shader) {
        shader = new PointLightShadowShader();
    }
    return shader;
}

void PointLightShadowShader::vertexShader(VertexP &vertex) {
    vertex.screen = uniform.VP * vertex.position;
    vertex.zRec = 1.0f / vertex.screen.w();
}

void PointLightShadowShader::destroy() {
    // uniform = nullptr;
    delete shader;
    shader = nullptr;
}

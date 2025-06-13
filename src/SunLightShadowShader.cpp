//
// Created by 闻永言 on 2021/12/23.
//

#include "SunLightShadowShader.hpp"
#include "Util.hpp"

using namespace std;

SunLightShadowShader *SunLightShadowShader::shader = nullptr;

SunLightShadowShader::~SunLightShadowShader() = default;

SunLightShadowShader *SunLightShadowShader::instance() {
    if (!shader) {
        shader = new SunLightShadowShader();
    }
    return shader;
}

void SunLightShadowShader::vertexShader(VertexP &vertex) {
    vertex.screen = uniform.matrixWorldToScreen * vertex.position;
}

void SunLightShadowShader::destroy() {
    // uniform = nullptr;
    delete shader;
    shader = nullptr;
}

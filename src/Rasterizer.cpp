//
// Created by 闻永言 on 2021/8/8.
//

#include "Rasterizer.hpp"
#include "Material.hpp"
#include "iodata.hpp"
#include "Shader.hpp"
#include "Util.hpp"
#include "Eigen/Geometry"
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/time.h>

using namespace std;
using namespace Renderer;

void Rasterizer::loadConfig(const string &modelConfig, const string &renderingConfig) {
    cout << "read config... " << endl;
    struct timeval t0, t1;
    double start_time, end_time, cost;
    // initialize state firstly
    gettimeofday(&t0, NULL);

    s = new State();
    iodata::renderingConfigParser(renderingConfig, s);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load config cost time: " << cost << " ms" << endl;
    cout << "view angle: " << rad2deg(s->camera->FovH) << endl;
    cout << "clipping plane [near, far]: " << s->camera->n << ", " << s->camera->f << endl;

    gettimeofday(&t0, NULL);

    s->model = iodata::modelConfigParser(modelConfig);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load model cost time: " << cost << " ms" << endl;
}

void Rasterizer::initialize() const {
    cout << "initialize..." << endl;
    if (s->mipmap) {
        for (auto &mesh: s->model->meshes) {
            mesh->material->textureBase->initializeMipmap();
            mesh->material->textureNormal->initializeMipmap();
            mesh->material->textureAO->initializeMipmap();
            mesh->material->textureMetalness->initializeMipmap();
            mesh->material->textureRoughness->initializeMipmap();
            mesh->material->textureEmission->initializeMipmap();
        }
    }

    // calculate vertex normal
    for (auto &mesh: s->model->meshes) {
        auto *normalSum = new float4[mesh->numVertices]; // sum of triangle normal vector contains vertex
        for (int i = 0; i < mesh->numVertices; i++) {
            normalSum[i] << 0, 0, 0, 0;
        }
        for (const auto &tri: mesh->triangles) {
            float4 OA = mesh->vertices[tri.vertexIndex[1]].position - mesh->vertices[tri.vertexIndex[0]].position;
            float4 OB = mesh->vertices[tri.vertexIndex[2]].position - mesh->vertices[tri.vertexIndex[0]].position;
            float4 normal = OA.cross3(OB).normalized(); // normalize
            normalSum[tri.vertexIndex[0]] += normal;
            normalSum[tri.vertexIndex[1]] += normal;
            normalSum[tri.vertexIndex[2]] += normal;
        }
        for (int i = 0; i < mesh->numVertices; i++) {
            mesh->vertices[i].normal = normalSum[i].normalized();
        }

        delete[] normalSum;
    }

    // calculate vertex tangent
    for (auto &mesh: s->model->meshes) {
        if (mesh->material->textureNormal) {
            auto *tangentSum = new float4[mesh->numVertices]; // sum of tangent vector of vertices in each face
            for (int i = 0; i < mesh->numVertices; i++) {
                tangentSum[i] << 0, 0, 0, 0;
            }
            for (const auto &tri: mesh->triangles) {
                float4 tangent =
                        calculateTangent(mesh->vertices[tri.vertexIndex[0]], mesh->vertices[tri.vertexIndex[1]],
                                         mesh->vertices[tri.vertexIndex[2]]);
                tangentSum[tri.vertexIndex[0]] += tangent;
                tangentSum[tri.vertexIndex[1]] += tangent;
                tangentSum[tri.vertexIndex[2]] += tangent;
            }
            for (int i = 0; i < mesh->numVertices; i++) {
                mesh->vertices[i].tangent = tangentSum[i].normalized();
            }

            delete[] tangentSum;
        }
    }

    s->check();
}

void Rasterizer::render(Renderer::RenderMode renderMode) {
    struct timeval t0, t1;
    double start_time, end_time;

    gettimeofday(&t0, NULL);
    initialize();
    gettimeofday(&t1, NULL);
    start_time = t0.tv_usec / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000;
    double cost = end_time - start_time;
    cout << "initialize cost time: " << cost << " ms" << endl;

    if (renderMode == Renderer::DO_LOOP) {
        renderLoop();
    } else if (renderMode == Renderer::OUTPUT_SINGLE) {
        renderSingle();
    } else if (renderMode == Renderer::ANIMATION) {
        renderAnimation();
    }
}

void Rasterizer::renderSingle() {
    struct timeval start, end;
    double start_time, end_time;

    //    s->num_threads = omp_get_max_threads();
    //    s->num_threads = 4;
    //    cout << "max_threads: " << s->num_threads << endl;
    gettimeofday(&start, NULL); // start

    Uniform uniform = Uniform();
    uniform.lightSource = &s->lightSource;
    uniform.mipmap = s->mipmap;
    uniform.samplerType = s->sampler;
    uniform.shadow = s->shadow;
    // shadow pass
    if (s->shadow == SHADOW) {
        for (const auto *light: s->lightSource) {
            if (light->updateShadow) {
                switch (light->type) {
                    case SUN: {
                        auto *sun = (SunLight *) light;
                        s->screenSize << sun->shadowSize, sun->shadowSize;
                        s->depthTest = true;
                        s->projection = ORTHO;
                        s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
                        frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
                        s->resetBuffer(); // initialize buffers
                        frameBuffer->reset();
                        uniform.setup(sun->position, sun->matrixOrthographic, sun->matrixViewport,
                                      sun->matrixWorldToScreen);
                        s->shader = reinterpret_cast<Shader *>(sun->shader); // set shadow shader
                        s->shader->setUniform(uniform);
                        drawScene(uniform, SHADOW_PASS);
                        sun->setShadowMap(s->zBuffer);
                        s->zBuffer = nullptr; // no delete
                        delete frameBuffer;
                        break;
                    }
                    case POINT:
                        auto *point = (PointLight *) light;
                        s->screenSize << point->shadowSize, point->shadowSize;
                        s->depthTest = true;
                        s->projection = PERS;
                        s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
                        frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
                        s->resetBuffer(); // initialize buffers
                        frameBuffer->reset();
                        s->shader = reinterpret_cast<Shader *>(point->shader); // set shadow shader
                        for (int i = 0; i < 6; i++) {
                            uniform.setup(point->position, point->VP[i], point->matrixViewport,
                                          point->matrixWorldToScreen[i]);
                            s->shader->setUniform(uniform);
                            drawScene(uniform, SHADOW_PASS);
                            point->setShadowMap(i, s->zBuffer);
                            s->zBuffer->reset(1.f);
                        }
                        delete frameBuffer;
                        break;
                }
            }
        }
    }

    // scene pass
    s->screenSize << s->camera->x, s->camera->y;
    s->projection = PERS;
    s->depthTest = true;
    s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
    frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
    s->resetBuffer(); // initialize buffers
    frameBuffer->reset();
    uniform.setup(s->camera->position, s->camera->matrixVP, s->camera->matrixViewport, s->camera->matrixWorldToScreen);
    drawScene(uniform, SCENE_PASS);
    dither();

    gettimeofday(&end, NULL);
    start_time = double(start.tv_usec) / 1000.0;
    end_time = double(end.tv_sec - start.tv_sec) * 1000.0 + double(end.tv_usec) / 1000.0;
    double cost = end_time - start_time;
    cout << "frame cost time: " << cost << " ms" << endl;
    cout << "fps: " << fixed << setprecision(1) << 1000.0 / cost << endl;

    iodata::writeResultImage(frameBuffer);
    delete frameBuffer;
}

void Rasterizer::renderLoop() {
    struct timeval start, end;
    unsigned long f0 = 0;
    unsigned long f1 = 1;

    Uniform uniform = Uniform();
    uniform.lightSource = &s->lightSource;
    uniform.mipmap = s->mipmap;
    uniform.samplerType = s->sampler;
    uniform.shadow = s->shadow;

    while (!stop) {
        if (f0 % 31 == 0) {
            gettimeofday(&start, NULL);
        }
        if (s->shadow == SHADOW) {
            for (const auto *light: s->lightSource) {
                if (light->updateShadow) {
                    switch (light->type) {
                        case SUN: {
                            auto *sun = (SunLight *) light;
                            s->screenSize << sun->shadowSize, sun->shadowSize;
                            s->projection = ORTHO;
                            s->depthTest = true;
                            s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
                            frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
                            s->resetBuffer(); // initialize buffers
                            frameBuffer->reset();
                            uniform.setup(sun->position, sun->matrixOrthographic, sun->matrixViewport,
                                          sun->matrixWorldToScreen);
                            s->shader = reinterpret_cast<Shader *>(sun->shader); // set shadow shader
                            s->shader->setUniform(uniform);
                            drawScene(uniform, SHADOW_PASS);
                            sun->setShadowMap(s->zBuffer);
                            s->zBuffer = nullptr;
                            delete frameBuffer;
                            break;
                        }
                        case POINT:
                            auto *point = (PointLight *) light;
                            s->screenSize << point->shadowSize, point->shadowSize;
                            s->projection = PERS;
                            s->depthTest = true;
                            s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
                            frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
                            s->resetBuffer(); // initialize buffers
                            frameBuffer->reset();
                            s->shader = reinterpret_cast<Shader *>(point->shader); // set shadow shader
                            for (int i = 0; i < 6; i++) {
                                uniform.setup(point->position, point->VP[i], point->matrixViewport,
                                              point->matrixWorldToScreen[i]);
                                s->shader->setUniform(uniform);
                                drawScene(uniform, SHADOW_PASS);
                                point->setShadowMap(i, s->zBuffer);
                                s->zBuffer->reset(1.f);
                            }
                            delete frameBuffer;
                            break;
                    }
                }
            }
        }

        s->screenSize << s->camera->x, s->camera->y;
        s->projection = PERS;
        s->depthTest = true;
        s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
        frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
        s->resetBuffer(); // initialize buffers
        frameBuffer->reset();
        uniform.setup(s->camera->position, s->camera->matrixVP, s->camera->matrixViewport,
                      s->camera->matrixWorldToScreen);
        drawScene(uniform, SCENE_PASS);
        dither();
        delete s->zBuffer;
        delete frameBuffer;

        if (f1 % 31 == 0) {
            gettimeofday(&end, NULL);
            double start_time = double(start.tv_usec) / 1000.0;
            double end_time = double(end.tv_sec - start.tv_sec) * 1000.0 + double(end.tv_usec) / 1000.0;
            cout << "FPS: " << fixed << setprecision(1) << 30000.0 / (end_time - start_time) << endl;
        }

        f0++;
        f1++;
    }
}

void Rasterizer::renderAnimation() {
    Uniform uniform = Uniform();
    uniform.lightSource = &s->lightSource;
    uniform.mipmap = s->mipmap;
    uniform.samplerType = s->sampler;
    uniform.shadow = s->shadow;

    SunLight *sun = (SunLight *) s->lightSource[0];
    float4 sun_center = sun->position;
    float4 center = s->camera->position;
    float4 focal = s->camera->focal;
    float4 up = s->camera->up;
    float distance = (center - focal).lpNorm<2>();
    float light_distance = (sun_center - focal).lpNorm<2>();

    int count = 36;
    int frame = 0;
    float z = distance * tanf(deg2rad(30.f));
    float radian = deg2rad(360.0f / (float) count);

    stringstream ss;
    string number;
    string anime_config = "Data/anime_config.txt";
    ofstream out;
    out.open(anime_config);
    if (!out.is_open()) {
        cerr << "CONFIG FILE OPEN FAILED" << endl;
        exit(0);
    }

    // output inner parameters
    out << count << endl;
    out << s->camera->x << " " << s->camera->y << " " << s->camera->ccdX << " " << s->camera->ccdY << " "
            << s->camera->focalLength << endl;

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < count; i++) {
            ss.clear();
            ss << setw(4) << setfill('0') << frame;
            ss >> number;
            string file_name = "Data/Animation/img_" + number + ".png";
            center << distance * cos(radian * (float) i), distance * sin(radian * (float) i), ((float) j - 1.f) * z,
                    1.0f;
            sun_center << light_distance * cosf(radian * ((float) i + (float) (random() % 200) / 200.0f)),
                    light_distance * sinf(radian * ((float) i + (float) (random() % 200) / 200.0f)), 0, 1.0f;
            s->camera->setLookAt(center, focal, up);
            sun->setLookAt(sun_center, focal, up);

            // shadow pass
            if (s->shadow == SHADOW) {
                for (const auto *light: s->lightSource) {
                    if (light->updateShadow) {
                        switch (light->type) {
                            case SUN: {
                                auto *sun = (SunLight *) light;
                                s->screenSize << sun->shadowSize, sun->shadowSize;
                                s->projection = ORTHO;
                                s->depthTest = true;
                                s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
                                frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
                                s->resetBuffer(); // initialize buffers
                                frameBuffer->reset();
                                uniform.setup(sun->position, sun->matrixOrthographic, sun->matrixViewport,
                                              sun->matrixWorldToScreen);
                                s->shader = reinterpret_cast<Shader *>(sun->shader); // set shadow shader
                                s->shader->setUniform(uniform);
                                drawScene(uniform, SHADOW_PASS);
                                sun->setShadowMap(s->zBuffer);
                                s->zBuffer = nullptr;
                                delete frameBuffer;
                                break;
                            }
                            case POINT:
                                auto *point = (PointLight *) light;
                                s->screenSize << point->shadowSize, point->shadowSize;
                                s->projection = PERS;
                                s->depthTest = true;
                                s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
                                frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
                                s->resetBuffer(); // initialize buffers
                                frameBuffer->reset();
                                s->shader = reinterpret_cast<Shader *>(point->shader); // set shadow shader
                                for (int face = 0; face < 6; face++) {
                                    uniform.setup(point->position, point->VP[face], point->matrixViewport,
                                                  point->matrixWorldToScreen[face]);
                                    s->shader->setUniform(uniform);
                                    drawScene(uniform, SHADOW_PASS);
                                    point->setShadowMap(face, s->zBuffer);
                                    s->zBuffer->reset(1.f);
                                }
                                delete frameBuffer;
                                break;
                        }
                    }
                }
            }
            // scene pass
            s->screenSize << s->camera->x, s->camera->y;
            s->projection = PERS;
            s->depthTest = true;
            s->zBuffer = new Image<float>(s->screenSize.x(), s->screenSize.y(), 1);
            frameBuffer = new FrameBuffer(s->screenSize.x(), s->screenSize.y());
            s->resetBuffer(); // initialize buffers
            frameBuffer->reset();
            uniform.setup(s->camera->position, s->camera->matrixVP, s->camera->matrixViewport,
                          s->camera->matrixWorldToScreen);
            drawScene(uniform, SCENE_PASS);
            dither();
            iodata::writeResultImage(file_name, frameBuffer);
            delete s->zBuffer;
            delete frameBuffer;

            out << file_name << fixed << setprecision(7) << " " << s->camera->matrixView(0, 0) << " "
                    << s->camera->matrixView(0, 1) << " " << s->camera->matrixView(0, 2) << " "
                    << s->camera->matrixView(1, 0) << " " << s->camera->matrixView(1, 1) << " "
                    << s->camera->matrixView(1, 2) << " " << s->camera->matrixView(2, 0) << " "
                    << s->camera->matrixView(2, 1) << " " << s->camera->matrixView(2, 2) << " "
                    << s->camera->matrixView(0, 3) << " " << s->camera->matrixView(1, 3) << " "
                    << s->camera->matrixView(2, 3) << endl;
            frame++;
        }
    }
    out.close();
}

inline bool Rasterizer::clipping(const VertexP &v0, const VertexP &v1, const VertexP &v2) {
    //    if (v_0.clip.w() < 1e-5 || v_1.clip.w() < 1e-5 || v_2.clip.w() < 1e-5) // to avoid divide by 0
    //    {
    //        return true;
    //    }
    if (v0.screen.x() < -v0.screen.w() && v1.screen.x() < -v1.screen.w() && v2.screen.x() < -v2.screen.w()) {
        return true;
    }
    if (v0.screen.x() > v0.screen.w() && v1.screen.x() > v1.screen.w() && v2.screen.x() > v2.screen.w()) {
        return true;
    }
    if (v0.screen.y() < -v0.screen.w() && v1.screen.y() < -v1.screen.w() && v2.screen.y() < -v2.screen.w()) {
        return true;
    }
    if (v0.screen.y() > v0.screen.w() && v1.screen.y() > v1.screen.w() && v2.screen.y() > v2.screen.w()) {
        return true;
    }
    if (v0.screen.z() < 0 || v1.screen.z() < 0 || v2.screen.z() < 0) {
        return true;
    }
    if (v0.screen.z() > v0.screen.w() && v1.screen.z() > v1.screen.w() && v2.screen.z() > v2.screen.w()) {
        return true;
    }
    return false;
}

vector<VertexP> Rasterizer::clipNear(const VertexP &v0, const VertexP &v1, const VertexP &v2) {
    VertexP vertices[3] = {v0, v1, v2};
    vector<VertexP> output;
    for (int i = 0; i < 3; i++) {
        const VertexP &start = vertices[i];
        const VertexP &end = vertices[(i + 1) % 3];

        if (end.screen.z() > 0) {
            if (start.screen.z() < 0) {
                float a = start.screen.z();
                float b = end.screen.z();
                float t = b / (b - a);
                output.push_back(lerp(t, end, start));
            }
            output.push_back(end);
        } else if (start.screen.z() > 0) {
            float a = end.screen.z();
            float b = start.screen.z();
            float t = b / (b - a);
            output.push_back(lerp(t, start, end));
        }
    }
    return output;
}

void Rasterizer::drawScene(Uniform &uniform, Pass pass) {
    for (auto mesh: s->model->meshes) {
        if (pass == SCENE_PASS) {
            mesh->material->setUniform(uniform);
            s->shader = mesh->material->getShader(); // set shader for material
            s->shader->setUniform(uniform); // set uniform
        }

#pragma omp parallel for
        for (const auto &tri: mesh->triangles) {
            auto v_0 = VertexP(mesh->vertices[tri.vertexIndex[0]]);
            auto v_1 = VertexP(mesh->vertices[tri.vertexIndex[1]]);
            auto v_2 = VertexP(mesh->vertices[tri.vertexIndex[2]]);
            s->shader->vertexShader(v_0);
            s->shader->vertexShader(v_1);
            s->shader->vertexShader(v_2);

            if (s->projection == ORTHO || !clipping(v_0, v_1, v_2)) {
                if (s->projection == PERS) {
                    // perspective division
                    perspectiveDivision(v_0);
                    perspectiveDivision(v_1);
                    perspectiveDivision(v_2);
                    // screen space coordinate
                    v_0.screen = uniform.matrixViewport * v_0.screen * v_0.zRec;
                    v_1.screen = uniform.matrixViewport * v_1.screen * v_1.zRec;
                    v_2.screen = uniform.matrixViewport * v_2.screen * v_2.zRec;
                }

                // face culling
                if (s->faceCullMode == BACK) {
                    float AB_x = v_1.screen.x() - v_0.screen.x();
                    float AB_y = v_1.screen.y() - v_0.screen.y();
                    float AC_x = v_2.screen.x() - v_0.screen.x();
                    float AC_y = v_2.screen.y() - v_0.screen.y();
                    if (AB_x * AC_y - AB_y * AC_x > 0) {
                        continue;
                    }
                } else if (s->faceCullMode == FRONT) {
                    float AB_x = v_1.screen.x() - v_0.screen.x();
                    float AB_y = v_1.screen.y() - v_0.screen.y();
                    float AC_x = v_2.screen.x() - v_0.screen.x();
                    float AC_y = v_2.screen.y() - v_0.screen.y();
                    if (AB_x * AC_y - AB_y * AC_x < 0) {
                        continue;
                    }
                }
                drawTriangle(v_0, v_1, v_2);
            } else {
                if (s->projection == PERS) // clip only in perspective projection
                {
                    vector<VertexP> vertices = clipNear(v_0, v_1, v_2);
                    for (auto &v: vertices) {
                        perspectiveDivision(v);
                        v.screen = uniform.matrixViewport * v.screen * v.zRec;
                    }

                    int numClipTri = vertices.size();
                    for (int j = 0; j < numClipTri; j += 2) {
                        const VertexP &v0 = vertices[j % numClipTri];
                        const VertexP &v1 = vertices[(j + 1) % numClipTri];
                        const VertexP &v2 = vertices[(j + 2) % numClipTri];

                        // face culling
                        if (s->faceCullMode == BACK) {
                            float AB_x = v1.screen.x() - v0.screen.x();
                            float AB_y = v1.screen.y() - v0.screen.y();
                            float AC_x = v2.screen.x() - v0.screen.x();
                            float AC_y = v2.screen.y() - v0.screen.y();
                            if (AB_x * AC_y - AB_y * AC_x > 0) {
                                continue;
                            }
                        } else if (s->faceCullMode == FRONT) {
                            float AB_x = v1.screen.x() - v0.screen.x();
                            float AB_y = v1.screen.y() - v0.screen.y();
                            float AC_x = v2.screen.x() - v0.screen.x();
                            float AC_y = v2.screen.y() - v0.screen.y();
                            if (AB_x * AC_y - AB_y * AC_x < 0) {
                                continue;
                            }
                        }
                        drawTriangle(v0, v1, v2);
                    }
                }
            }
        }
    }
}

void Rasterizer::drawTriangle(const VertexP &v0, const VertexP &v1, const VertexP &v2) {
    float4 OA = v1.position - v0.position;
    float4 OB = v2.position - v0.position;
    float4 flatNormal = OA.cross3(OB).normalized();

    // real bounding box
    int min_x = max((int) min(v0.screen.x(), min(v1.screen.x(), v2.screen.x())), 0);
    int min_y = max((int) min(v0.screen.y(), min(v1.screen.y(), v2.screen.y())), 0);
    int max_x = min((int) max(v0.screen.x(), max(v1.screen.x(), v2.screen.x())) + 1, s->screenSize.x() - 1);
    int max_y = min((int) max(v0.screen.y(), max(v1.screen.y(), v2.screen.y())) + 1, s->screenSize.y() - 1);

    int index = min_y * s->screenSize.x();
    for (int i = min_y; i <= max_y; i++) {
        for (int j = min_x; j <= max_x; j++) {
            float center_x = float(j) + 0.5f;
            float center_y = float(i) + 0.5f;
            float v0x = v0.screen.x() - center_x;
            float v0y = v0.screen.y() - center_y;
            float v1x = v1.screen.x() - center_x;
            float v1y = v1.screen.y() - center_y;
            float v2x = v2.screen.x() - center_x;
            float v2y = v2.screen.y() - center_y;
            float AB = v1x * v0y - v1y * v0x;
            float BC = v2x * v1y - v2y * v1x;
            float CA = v0x * v2y - v0y * v2x;
            if (AB > 0 && BC > 0 && CA > 0) {
                float S = 1.0f / (AB + BC + CA);
                float u = BC * S;
                float v = CA * S;
                float w = AB * S;
                float z = lerp(v0.screen.z(), v1.screen.z(), v2.screen.z(), u, v, w);
                // early-z test
                if (s->depthTest && z < s->zBuffer->data[index + j]) {
                    // perspective correct interpolation
                    Fragment frag = lerp(v0, v1, v2, u, v, w);
                    frag.flatNormal = flatNormal;
                    // calculate texture space mapping
                    if (s->mipmap) {
                        center_x = float(j) + 1.5f;
                        center_y = float(i) + 0.5f;
                        v0x = v0.screen.x() - center_x;
                        v0y = v0.screen.y() - center_y;
                        v1x = v1.screen.x() - center_x;
                        v1y = v1.screen.y() - center_y;
                        v2x = v2.screen.x() - center_x;
                        v2y = v2.screen.y() - center_y;
                        AB = v1x * v0y - v1y * v0x;
                        BC = v2x * v1y - v2y * v1x;
                        CA = v0x * v2y - v0y * v2x;
                        S = 1.0f / (AB + BC + CA);
                        u = BC * S;
                        v = CA * S;
                        w = AB * S;
                        frag.ddx = lerp(v0.textureCoord, v1.textureCoord, v2.textureCoord, u, v, w);

                        center_x = float(j) + 0.5f;
                        center_y = float(i) + 1.5f;
                        v0x = v0.screen.x() - center_x;
                        v0y = v0.screen.y() - center_y;
                        v1x = v1.screen.x() - center_x;
                        v1y = v1.screen.y() - center_y;
                        v2x = v2.screen.x() - center_x;
                        v2y = v2.screen.y() - center_y;
                        AB = v1x * v0y - v1y * v0x;
                        BC = v2x * v1y - v2y * v1x;
                        CA = v0x * v2y - v0y * v2x;
                        S = 1.0f / (AB + BC + CA);
                        u = BC * S;
                        v = CA * S;
                        w = AB * S;
                        frag.ddy = lerp(v0.textureCoord, v1.textureCoord, v2.textureCoord, u, v, w);
                    }

                    // fragment perspective restore
                    if (s->projection == PERS) {
                        perspectiveRestore(frag);
                    }
                    writeColor(j, i, z, s->shader->fragmentShader(frag));
                }
            }
        }
        index += s->screenSize.x();
    }
}

inline void Rasterizer::alphaTest() {
    //    cout << "alpha shading..." << endl;
}

inline void Rasterizer::stencilTest() {
    //    cout << "stencil test..." << endl;
}

inline void Rasterizer::writeColor(int _x, int _y, float z, const float4 &color) const {
    int index = _y * s->screenSize.x() + _x;
    if (s->depthTest) // z test
    {
        if (z < s->zBuffer->data[index]) {
            s->zBuffer->data[index] = z;
            frameBuffer->writeColor(index, color);
        }
    } else {
        frameBuffer->writeColor(index, color);
    }
}

inline void Rasterizer::dither() {
    //    cout << "dither..." << endl;
}

inline void Rasterizer::frameBlend() {
    //    cout << "frame blend..." << endl;
}

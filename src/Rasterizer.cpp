//
// Created by 闻永言 on 2021/8/8.
//

#include "Rasterizer.hpp"
#include "Material.hpp"
#include "iodata.hpp"
#include "Mipmap.hpp"
#include "Shader.hpp"
#include "Util.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/time.h>

using namespace std;
using namespace Render;

int num_frag = 0;
int num_triangle = 0;

void Rasterizer::readData(const string &modelFile, const string &configFile)
{
    cout << "read data... " << endl;
    struct timeval t0, t1;
    double start_time, end_time, cost;
    // initialize state firstly
    gettimeofday(&t0, NULL);

    s = iodata::loadConfig(configFile);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load config cost time: " << cost << " ms" << endl;
    cout << "view angle: " << rad2deg(s->camera->FovH) << endl;
    cout << "clipping plane [near, far]: " << s->camera->n << ", " << s->camera->f << endl;
    // read data after read config, otherwise state is not exist
    //    iodata::read_DEM(model_file, s->model, DEM_x, DEM_y);

    gettimeofday(&t0, NULL);

    iodata::readPly(modelFile, s->model);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load model cost time: " << cost << " ms" << endl;
    //    iodata::write_ply(&s->model->meshes[0]); // write ply file
}

void Rasterizer::initialize() const
{
    cout << "initialize..." << endl;
    int x = s->camera->x, y = s->camera->y;
    //    s->stencil_buffer = new float[x * y];
    s->zBuffer = new float[x * y];
    s->colorBuffer = new float4[x * y];
    s->frameBuffer = new FrameBuffer(x, y);
    s->resetBuffer(); // initialize buffers

    //    s->num_threads = omp_get_max_threads();
    //    s->num_threads = 4;
    //    cout << "max_threads: " << s->num_threads << endl;

    if (s->shadow == SHADOW)
    {
        cout << "shadow mapping..." << endl;
        for (auto l: s->lightSource)
        {
            l->shadowMapping(s->model); // calculate shadow map
        }
    }

    // generate mipmap with base texture type
    s->check();
    if (s->textureType == MIPMAP)
    {
        for (auto &mesh: s->model->meshes)
        {
            mesh->material->baseTexture->initializeMipmap();
        }
    }
}

void Rasterizer::render(Render::RenderMode renderMode)
{
    struct timeval start, end, t0, t1;
    double start_time, end_time;

    gettimeofday(&t0, NULL);
    initialize();
    gettimeofday(&t1, NULL);
    start_time = t0.tv_usec / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000;
    double cost = end_time - start_time;
    cout << "initialize cost time: " << cost << " ms" << endl;

    if (renderMode == Render::DO_LOOP)
    {
        renderLoop();
    } else if (renderMode == Render::OUTPUT_SINGLE)
    {
        num_frag = 0;
        num_triangle = 0;
        gettimeofday(&start, NULL); // start
        //        gettimeofday(&t0, NULL);
        //        view_transform();
        //        gettimeofday(&t1, NULL);
        //        start_time = double(t0.tv_usec) / 1000.0;
        //        end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
        //        cost = end_time - start_time;
        //        cout << "view transform cost time: " << cost << " ms" << endl;

        //        gettimeofday(&t0, NULL);
        draw();
        //        gettimeofday(&t1, NULL);
        //        start_time = double(t0.tv_usec) / 1000.0;
        //        end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
        //        cost = end_time - start_time;
        //        cout << "draw cost time: " << cost << " ms" << endl;

        //        gettimeofday(&t0, NULL);
        dither();
        //        frame_blend();
        //        gettimeofday(&t1, NULL);
        //        start_time = double(t0.tv_usec) / 1000.0;
        //        end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
        //        cost = end_time - start_time;
        //        cout << "frame blend cost time: " << cost << " ms" << endl;

        gettimeofday(&end, NULL);
        start_time = double(start.tv_usec) / 1000.0;
        end_time = double(end.tv_sec - start.tv_sec) * 1000.0 + double(end.tv_usec) / 1000.0;
        cost = end_time - start_time;
        cout << "fragment: " << num_frag << endl;
        cout << "triangle: " << num_triangle << endl;
        cout << "frame cost time: " << cost << " ms" << endl;
        cout << "fps: " << fixed << setprecision(1) << 1000.0 / cost << endl;

        iodata::writeResultImage(s->frameBuffer);
    } else if (renderMode == Render::ANIMATION)
    {
        SunLight *sun = (SunLight *)s->lightSource[0];
        float4 sun_center = sun->position;
        float4 center = s->camera->position;
        float4 focal = s->camera->focal;
        float4 up = s->camera->up;
        float distance = (center - focal).lpNorm<2>();
        float light_distance = (sun_center - focal).lpNorm<2>();

        int count = 36;
        int frame = 0;
        float radian = deg2rad(360.0f / (float)count);

        stringstream ss;
        string number;
        string anime_config = "Data/anime_config.txt";
        ofstream out;
        out.open(anime_config);
        if (!out.is_open())
        {
            cerr << "CONFIG FILE OPEN FAILED" << endl;
            exit(0);
        }

        // output inner parameters
        out << count << endl;
        out << s->camera->x << " " << s->camera->y << " " << s->camera->ccdX << " " << s->camera->ccdY << " "
            << s->camera->focalLength << endl;
        for (int i = 0; i < count; i++)
        {
            ss.clear();
            ss << setw(4) << setfill('0') << frame;
            ss >> number;
            string file_name = "Data/Animation/img_" + number + ".png";
            center << distance * cos(radian * (float)i), distance * sin(radian * (float)i), 0, 1.0f;
            sun_center << light_distance * cos(radian * ((float)i + (float)(rand() % 200) / 50.0f)), light_distance *
                                                                                                     sin(
                                                                                                             radian *
                                                                                                             ((float)i +
                                                                                                              (float)(rand() %
                                                                                                                      200) /
                                                                                                              50.0f)), 0, 1.0f;
            s->camera->setLookAt(center, focal, up);
            sun->setLookAt(sun_center, focal, up);
            if (s->shadow == SHADOW)
            {
                sun->shadowMapping(s->model); // update shadow map
            }
            draw();
            out << file_name << fixed << setprecision(7) << " " << s->camera->matrixView(0, 0) << " "
                << s->camera->matrixView(0, 1) << " " << s->camera->matrixView(0, 2) << " "
                << s->camera->matrixView(1, 0) << " " << s->camera->matrixView(1, 1) << " "
                << s->camera->matrixView(1, 2) << " " << s->camera->matrixView(2, 0) << " "
                << s->camera->matrixView(2, 1) << " " << s->camera->matrixView(2, 2) << " "
                << s->camera->matrixView(0, 3) << " " << s->camera->matrixView(1, 3) << " "
                << s->camera->matrixView(2, 3) << endl;
            iodata::writeResultImage(file_name, s->frameBuffer);
            s->resetBuffer();
            frame++;
        }

        out.close();
    }
}

void Rasterizer::renderLoop()
{
    while (!stop)
    {
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0; i < 30; i++)
        {
            //            view_transform();
            draw();
            dither();
            //            frame_blend();
            s->resetBuffer(); // reset buffer
        }
        gettimeofday(&end, NULL);
        double start_time = double(start.tv_usec) / 1000.0;
        double end_time = double(end.tv_sec - start.tv_sec) * 1000.0 + double(end.tv_usec) / 1000.0;
        cout << "fps: " << fixed << setprecision(1) << 30000.0 / (end_time - start_time) << endl;
    }
}

//void Rasterizer::view_transform() const
//{
//    //    cout << "view transform..." << endl;
//
//    for (auto mesh: s->model->meshes)
//    {
//        //#pragma omp parallel for
//        for (int i = 0; i < mesh->num_triangle; i++)
//        {
//            mesh->triangles[i].normal = s->camera->Q * mesh->triangles[i].normal; // normal vector transform
//        }
//    }
//}

inline bool Rasterizer::clipping(const VertexP &v0, const VertexP &v1, const VertexP &v2)
{
    //    if (v_0.clip.w() < 1e-5 || v_1.clip.w() < 1e-5 || v_2.clip.w() < 1e-5) // to avoid divide by 0
    //    {
    //        return true;
    //    }
    if (v0.clip.x() < -v0.clip.w() && v1.clip.x() < -v1.clip.w() && v2.clip.x() < -v2.clip.w())
    {
        return true;
    }
    if (v0.clip.x() > v0.clip.w() && v1.clip.x() > v1.clip.w() && v2.clip.x() > v2.clip.w())
    {
        return true;
    }
    if (v0.clip.y() < -v0.clip.w() && v1.clip.y() < -v1.clip.w() && v2.clip.y() < -v2.clip.w())
    {
        return true;
    }
    if (v0.clip.y() > v0.clip.w() && v1.clip.y() > v1.clip.w() && v2.clip.y() > v2.clip.w())
    {
        return true;
    }
    if (v0.clip.z() < 0 || v1.clip.z() < 0 || v2.clip.z() < 0)
    {
        return true;
    }
    if (v0.clip.z() > v0.clip.w() && v1.clip.z() > v1.clip.w() && v2.clip.z() > v2.clip.w())
    {
        return true;
    }
    return false;
}

vector<VertexP> Rasterizer::clip_near(const VertexP &v0, const VertexP &v1, const VertexP &v2)
{
    VertexP vertices[3] = {v0, v1, v2};
    vector<VertexP> output;
    for (int i = 0; i < 3; i++)
    {
        const VertexP &start = vertices[i];
        const VertexP &end = vertices[(i + 1) % 3];

        if (end.clip.z() > 0)
        {
            if (start.clip.z() < 0)
            {
                float a = start.clip.z();
                float b = end.clip.z();
                float t = b / (b - a);
                output.push_back(lerp(t, end, start));
            }
            output.push_back(end);
        } else if (start.clip.z() > 0)
        {
            float a = end.clip.z();
            float b = start.clip.z();
            float t = b / (b - a);
            output.push_back(lerp(t, start, end));
        }
    }
    return output;
}

void Rasterizer::draw()
{
    for (auto mesh: s->model->meshes)
    {
        Uniform u = mesh->material->getUniform();
        s->shader = mesh->material->getShader(); // set shader for material
        s->shader->setUniform(&u); // set uniform
        s->shader->setLight(&s->lightSource); // set light source
        s->shader->setCamera(s->camera); // set camera
        s->shader->setTextureType(s->textureType); // set texture type
        s->shader->setSampler(s->sampler); // set sampler

        //#pragma omp parallel for
        //        for (int i = 0; i < mesh->num_vertex; i++)
        //        {
        //            s->shader->vertex_shader(mesh->vertices[i]);
        //            perspective_division(mesh->vertices[i]);
        //            mesh->vertices[i].screen = s->camera->M_viewport * mesh->vertices[i].screen;
        //        }

#pragma omp parallel for
        for (const auto &tri: mesh->triangles)
        {
            auto v_0 = VertexP(mesh->vertices[tri.vertexIndex[0]]);
            auto v_1 = VertexP(mesh->vertices[tri.vertexIndex[1]]);
            auto v_2 = VertexP(mesh->vertices[tri.vertexIndex[2]]);
            s->shader->vertexShader(v_0);
            s->shader->vertexShader(v_1);
            s->shader->vertexShader(v_2);

            if (!clipping(v_0, v_1, v_2))
            {
                // perspective division
                perspectiveDivision(v_0);
                perspectiveDivision(v_1);
                perspectiveDivision(v_2);
                // screen space coordinate
                v_0.screen = s->camera->matrixViewport * v_0.clip * v_0.z_rec;
                v_1.screen = s->camera->matrixViewport * v_1.clip * v_1.z_rec;
                v_2.screen = s->camera->matrixViewport * v_2.clip * v_2.z_rec;

                // face culling
                if (s->faceCullMode == BACK)
                {
                    float AB_x = v_1.screen.x() - v_0.screen.x();
                    float AB_y = v_1.screen.y() - v_0.screen.y();
                    float AC_x = v_2.screen.x() - v_0.screen.x();
                    float AC_y = v_2.screen.y() - v_0.screen.y();
                    if (AB_x * AC_y - AB_y * AC_x > 0)
                    {
                        continue;
                    }
                } else if (s->faceCullMode == FRONT)
                {
                    float AB_x = v_1.screen.x() - v_0.screen.x();
                    float AB_y = v_1.screen.y() - v_0.screen.y();
                    float AC_x = v_2.screen.x() - v_0.screen.x();
                    float AC_y = v_2.screen.y() - v_0.screen.y();
                    if (AB_x * AC_y - AB_y * AC_x < 0)
                    {
                        continue;
                    }
                }
                num_triangle++;
                draw_triangle(v_0, v_1, v_2, tri.normal);
            } else
            {
                vector<VertexP> vertices = clip_near(v_0, v_1, v_2);
                for (auto &v: vertices)
                {
                    perspectiveDivision(v);
                    v.screen = s->camera->matrixViewport * v.clip * v.z_rec;
                }

                int num_clip_tri = vertices.size();
                for (int j = 0; j < num_clip_tri; j += 2)
                {
                    const VertexP &v0 = vertices[j % num_clip_tri];
                    const VertexP &v1 = vertices[(j + 1) % num_clip_tri];
                    const VertexP &v2 = vertices[(j + 2) % num_clip_tri];

                    // face culling
                    if (s->faceCullMode == BACK)
                    {
                        float AB_x = v1.screen.x() - v0.screen.x();
                        float AB_y = v1.screen.y() - v0.screen.y();
                        float AC_x = v2.screen.x() - v0.screen.x();
                        float AC_y = v2.screen.y() - v0.screen.y();
                        if (AB_x * AC_y - AB_y * AC_x > 0)
                        {
                            continue;
                        }
                    } else if (s->faceCullMode == FRONT)
                    {
                        float AB_x = v1.screen.x() - v0.screen.x();
                        float AB_y = v1.screen.y() - v0.screen.y();
                        float AC_x = v2.screen.x() - v0.screen.x();
                        float AC_y = v2.screen.y() - v0.screen.y();
                        if (AB_x * AC_y - AB_y * AC_x < 0)
                        {
                            continue;
                        }
                    }
                    num_triangle++;
                    draw_triangle(v0, v1, v2, tri.normal);
                }
            }
        }
    }

    //#pragma omp parallel for
    //        for (int i = 0; i < mesh->num_vertex; i++)
    //        {
    //            perspective_restore(mesh->vertices[i]); // perspective restore
    //        }
}

void Rasterizer::draw_triangle(const VertexP &v0, const VertexP &v1, const VertexP &v2, const float4 &flatNormal)
{
    // real bounding box
    int min_x = max((int)min(v0.screen.x(), min(v1.screen.x(), v2.screen.x())), 0);
    int min_y = max((int)min(v0.screen.y(), min(v1.screen.y(), v2.screen.y())), 0);
    int max_x = min((int)max(v0.screen.x(), max(v1.screen.x(), v2.screen.x())) + 1, s->camera->x - 1);
    int max_y = min((int)max(v0.screen.y(), max(v1.screen.y(), v2.screen.y())) + 1, s->camera->y - 1);

    int index = min_y * s->camera->x;
    for (int i = min_y; i <= max_y; i++)
    {
        for (int j = min_x; j <= max_x; j++)
        {
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
            if (AB > 0 && BC > 0 && CA > 0)
            {
                float S = 1.0f / (AB + BC + CA);
                float u = BC * S;
                float v = CA * S;
                float w = AB * S;
                float z = lerp(v0.screen.z(), v1.screen.z(), v2.screen.z(), u, v, w);
                // early-z test
                if (z < s->zBuffer[index + j])
                {
                    num_frag++;
                    // perspective correct interpolation
                    Fragment frag = lerp(v0, v1, v2, u, v, w);
                    frag.flatNormal = flatNormal;

                    // calculate texture space mapping
                    if (s->textureType == MIPMAP)
                    {
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
                        frag.ddx = lerp(v0.texture_uv, v1.texture_uv, v2.texture_uv, u, v, w);

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
                        frag.ddy = lerp(v0.texture_uv, v1.texture_uv, v2.texture_uv, u, v, w);
                    }

                    // fragment perspective restore
                    perspectiveRestore(frag);
                    write_color(j, i, z, s->shader->fragmentShader(frag));
                }
            }
        }
        index += s->camera->x;
    }
}

inline void Rasterizer::alphaTest()
{
    //    cout << "alpha shading..." << endl;
}

inline void Rasterizer::stencilTest()
{
    //    cout << "stencil test..." << endl;
}

inline void Rasterizer::write_color(int _x, int _y, float z, const float4 &color) const
{
    int index = _y * s->camera->x + _x;
    if (z < s->zBuffer[index])
    {
        s->zBuffer[index] = z;
        s->frameBuffer->writeColor(index, color);
    }
}

inline void Rasterizer::dither()
{
    //    cout << "dither..." << endl;
}

inline void Rasterizer::frameBlend()
{
    //    cout << "frame blend..." << endl;
    //    for (int i = 0; i < s->camera->x * s->camera->y; i++)
    //    {
    //        s->frame_buffer->buffer[i] = s->color_buffer[i];
    //    }
}

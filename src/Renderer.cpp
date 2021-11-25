//
// Created by 闻永言 on 2021/8/8.
//

#include "Renderer.hpp"
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

int num_frag = 0;
int num_triangle = 0;

void Renderer::read_data(const string &model_file, const string &config_file)
{
    cout << "read data... " << endl;
    struct timeval t0, t1;
    double start_time, end_time, cost;
    // initialize state firstly
    gettimeofday(&t0, NULL);

    s = iodata::load_config(config_file);

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

    iodata::read_ply(model_file, s->model);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load model cost time: " << cost << " ms" << endl;
    //    iodata::write_ply(&s->model->meshes[0]); // write ply file
}

void Renderer::initialize() const
{
    cout << "initialize..." << endl;
    int x = s->camera->x, y = s->camera->y;
    s->stencil_buffer = new float[x * y];
    s->z_buffer = new float[x * y];
    s->color_buffer = new float4[x * y];
    s->frame_buffer = new FrameBuffer(x, y);
    s->reset_buffer(); // initialize buffers

    //    s->num_threads = omp_get_max_threads();
    //    s->num_threads = 4;
    //    cout << "max_threads: " << s->num_threads << endl;

    if (s->shadow == SHADOW)
    {
        cout << "shadow mapping..." << endl;
        for (auto l: s->light)
        {
            l->shadow_mapping(s->model); // calculate shadow map
        }
    }

    // generate mipmap with base texture type
    s->check();
    if (s->texture_type == MIPMAP)
    {
        for (auto &mesh: s->model->meshes)
        {
            mesh->material->base_texture->initialize_mipmap();
        }
    }
}

void Renderer::render(int mode)
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

    if (mode == Render::DO_LOOP)
    {
        render_loop();
    } else if (mode == Render::OUTPUT_SINGLE)
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

        iodata::write_result_image(*s->frame_buffer);
    } else if (mode == Render::ANIMATION)
    {
        SunLight *sun = (SunLight *)s->light[0];
        float4 sun_center = sun->center;
        float4 center = s->camera->camera_center;
        float4 focal = s->camera->focal_center;
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
        out << s->camera->x << " " << s->camera->y << " " << s->camera->ccd_x << " " << s->camera->ccd_y << " "
            << s->camera->focal << endl;
        for (int i = 0; i < count; i++)
        {
            ss.clear();
            ss << setw(4) << setfill('0') << frame;
            ss >> number;
            string file_name = "Data/Animation/img_" + number + ".png";
            center << distance * cos(radian * (float)i), distance * sin(radian * (float)i), 0, 1.0f;
            sun_center << light_distance * cos(radian * (float)i), light_distance * sin(radian * (float)i), 0, 1.0f;
            s->camera->set_look_at(center, focal, up);
            sun->set_look_at(sun_center, focal, up);
            if (s->shadow)
            {
                sun->shadow_mapping(s->model); // update shadow map
            }
            draw();

            out << file_name << fixed << setprecision(7) << " " << s->camera->M_view(0, 0) << " "
                << s->camera->M_view(0, 1) << " " << s->camera->M_view(0, 2) << " " << s->camera->M_view(1, 0) << " "
                << s->camera->M_view(1, 1) << " " << s->camera->M_view(1, 2) << " " << s->camera->M_view(2, 0) << " "
                << s->camera->M_view(2, 1) << " " << s->camera->M_view(2, 2) << " " << s->camera->M_view(0, 3) << " "
                << s->camera->M_view(1, 3) << " " << s->camera->M_view(2, 3) << endl;
            iodata::write_result_image(file_name, *s->frame_buffer);
            s->reset_buffer();
            frame++;
        }

        out.close();
    }
}

void Renderer::render_loop()
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
            s->reset_buffer(); // reset buffer
        }
        gettimeofday(&end, NULL);
        double start_time = double(start.tv_usec) / 1000.0;
        double end_time = double(end.tv_sec - start.tv_sec) * 1000.0 + double(end.tv_usec) / 1000.0;
        cout << "fps: " << fixed << setprecision(1) << 30000.0 / (end_time - start_time) << endl;
    }
}

void Renderer::view_transform()
{
    //    cout << "view transform..." << endl;

    for (auto mesh: s->model->meshes)
    {
        //#pragma omp parallel for
        for (int i = 0; i < mesh->num_triangle; i++)
        {
            mesh->triangles[i].normal = s->camera->Q * mesh->triangles[i].normal; // normal vector transform
        }
    }
}

inline bool Renderer::clipping(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2)
{
    //    if (v_0.clip.w() < 1e-5 || v_1.clip.w() < 1e-5 || v_2.clip.w() < 1e-5) // to avoid divide by 0
    //    {
    //        return true;
    //    }
    if (v_0.clip.x() < -v_0.clip.w() && v_1.clip.x() < -v_1.clip.w() && v_2.clip.x() < -v_2.clip.w())
    {
        return true;
    }
    if (v_0.clip.x() > v_0.clip.w() && v_1.clip.x() > v_1.clip.w() && v_2.clip.x() > v_2.clip.w())
    {
        return true;
    }
    if (v_0.clip.y() < -v_0.clip.w() && v_1.clip.y() < -v_1.clip.w() && v_2.clip.y() < -v_2.clip.w())
    {
        return true;
    }
    if (v_0.clip.y() > v_0.clip.w() && v_1.clip.y() > v_1.clip.w() && v_2.clip.y() > v_2.clip.w())
    {
        return true;
    }
    if (v_0.clip.z() < 0 || v_1.clip.z() < 0 || v_2.clip.z() < 0)
    {
        return true;
    }
    if (v_0.clip.z() > v_0.clip.w() && v_1.clip.z() > v_1.clip.w() && v_2.clip.z() > v_2.clip.w())
    {
        return true;
    }
    return false;
}

vector<VertexP> Renderer::clip_near(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2)
{
    VertexP vertices[3] = {v_0, v_1, v_2};
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

void Renderer::draw()
{
    for (auto mesh: s->model->meshes)
    {
        Uniform u = mesh->material->get_uniform();
        s->shader = mesh->material->get_shader(); // set shader for material
        s->shader->set_uniform(&u); // set uniform
        s->shader->set_light(&s->light); // set light source
        s->shader->set_camera(s->camera); // set camera
        s->shader->set_texture_type(s->texture_type); // set texture type
        s->shader->set_sampler(s->sampler); // set sampler

        //#pragma omp parallel for
        //        for (int i = 0; i < mesh->num_vertex; i++)
        //        {
        //            s->shader->vertex_shader(mesh->vertices[i]);
        //            perspective_division(mesh->vertices[i]);
        //            mesh->vertices[i].screen = s->camera->M_viewport * mesh->vertices[i].screen;
        //        }

#pragma omp parallel for
        for (int i = 0; i < mesh->num_triangle; i++)
        {
            auto v_0 = VertexP(mesh->vertices[mesh->triangles[i].vertex_0]);
            auto v_1 = VertexP(mesh->vertices[mesh->triangles[i].vertex_1]);
            auto v_2 = VertexP(mesh->vertices[mesh->triangles[i].vertex_2]);
            s->shader->vertex_shader(v_0);
            s->shader->vertex_shader(v_1);
            s->shader->vertex_shader(v_2);

            if (!clipping(v_0, v_1, v_2))
            {
                // perspective division
                perspective_division(v_0);
                perspective_division(v_1);
                perspective_division(v_2);
                // screen space coordinate
                v_0.screen = s->camera->M_viewport * v_0.clip * v_0.z_rec;
                v_1.screen = s->camera->M_viewport * v_1.clip * v_1.z_rec;
                v_2.screen = s->camera->M_viewport * v_2.clip * v_2.z_rec;

                // face culling
                if (s->face_cull_mode == BACK)
                {
                    float AB_x = v_1.screen.x() - v_0.screen.x();
                    float AB_y = v_1.screen.y() - v_0.screen.y();
                    float AC_x = v_2.screen.x() - v_0.screen.x();
                    float AC_y = v_2.screen.y() - v_0.screen.y();
                    if (AB_x * AC_y - AB_y * AC_x > 0)
                    {
                        continue;
                    }
                } else if (s->face_cull_mode == FRONT)
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
                draw_triangle(v_0, v_1, v_2, mesh->triangles[i].normal);
            } else
            {
                vector<VertexP> vertices = clip_near(v_0, v_1, v_2);
                for(auto &v: vertices)
                {
                    perspective_division(v);
                    v.screen = s->camera->M_viewport * v.clip * v.z_rec;
                }

                int num_clip_tri = vertices.size();
                for (int j = 0; j < num_clip_tri; j += 2)
                {
                    const VertexP &v0 = vertices[j % num_clip_tri];
                    const VertexP &v1 = vertices[(j + 1) % num_clip_tri];
                    const VertexP &v2 = vertices[(j + 2) % num_clip_tri];

                    // face culling
                    if (s->face_cull_mode == BACK)
                    {
                        float AB_x = v1.screen.x() - v0.screen.x();
                        float AB_y = v1.screen.y() - v0.screen.y();
                        float AC_x = v2.screen.x() - v0.screen.x();
                        float AC_y = v2.screen.y() - v0.screen.y();
                        if (AB_x * AC_y - AB_y * AC_x > 0)
                        {
                            continue;
                        }
                    } else if (s->face_cull_mode == FRONT)
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
                    draw_triangle(v0, v1, v2, mesh->triangles[i].normal);
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

void Renderer::draw_triangle(const VertexP &v_0, const VertexP &v_1, const VertexP &v_2, const float4 &normal)
{
    // real bounding box
    int min_x = max((int)min(v_0.screen.x(), min(v_1.screen.x(), v_2.screen.x())), 0);
    int min_y = max((int)min(v_0.screen.y(), min(v_1.screen.y(), v_2.screen.y())), 0);
    int max_x = min((int)max(v_0.screen.x(), max(v_1.screen.x(), v_2.screen.x())) + 1, s->camera->x - 1);
    int max_y = min((int)max(v_0.screen.y(), max(v_1.screen.y(), v_2.screen.y())) + 1, s->camera->y - 1);

    int index = min_y * s->camera->x;
    for (int i = min_y; i <= max_y; i++)
    {
        for (int j = min_x; j <= max_x; j++)
        {
            float center_x = float(j) + 0.5f;
            float center_y = float(i) + 0.5f;
            float v0x = v_0.screen.x() - center_x;
            float v0y = v_0.screen.y() - center_y;
            float v1x = v_1.screen.x() - center_x;
            float v1y = v_1.screen.y() - center_y;
            float v2x = v_2.screen.x() - center_x;
            float v2y = v_2.screen.y() - center_y;
            float AB = v1x * v0y - v1y * v0x;
            float BC = v2x * v1y - v2y * v1x;
            float CA = v0x * v2y - v0y * v2x;
            if (AB > 0 && BC > 0 && CA > 0)
            {
                float S = 1.0f / (AB + BC + CA);
                float u = BC * S;
                float v = CA * S;
                float w = AB * S;
                float z = lerp(v_0.screen.z(), v_1.screen.z(), v_2.screen.z(), u, v, w);
                // early-z test
                if (z < s->z_buffer[index + j])
                {
                    num_frag++;
                    // perspective correct interpolation
                    Fragment frag = lerp(v_0, v_1, v_2, u, v, w);
                    frag.x = j;
                    frag.y = i;
                    frag.z = z;
                    frag.flat_normal = normal;

                    // calculate texture space mapping
                    if (s->texture_type == MIPMAP)
                    {
                        center_x = float(j) + 1.5f;
                        center_y = float(i) + 0.5f;
                        v0x = v_0.screen.x() - center_x;
                        v0y = v_0.screen.y() - center_y;
                        v1x = v_1.screen.x() - center_x;
                        v1y = v_1.screen.y() - center_y;
                        v2x = v_2.screen.x() - center_x;
                        v2y = v_2.screen.y() - center_y;
                        AB = v1x * v0y - v1y * v0x;
                        BC = v2x * v1y - v2y * v1x;
                        CA = v0x * v2y - v0y * v2x;
                        S = 1.0f / (AB + BC + CA);
                        u = BC * S;
                        v = CA * S;
                        w = AB * S;
                        frag.texture_x = lerp(v_0.texture_uv, v_1.texture_uv, v_2.texture_uv, u, v, w);

                        center_x = float(j) + 0.5f;
                        center_y = float(i) + 1.5f;
                        v0x = v_0.screen.x() - center_x;
                        v0y = v_0.screen.y() - center_y;
                        v1x = v_1.screen.x() - center_x;
                        v1y = v_1.screen.y() - center_y;
                        v2x = v_2.screen.x() - center_x;
                        v2y = v_2.screen.y() - center_y;
                        AB = v1x * v0y - v1y * v0x;
                        BC = v2x * v1y - v2y * v1x;
                        CA = v0x * v2y - v0y * v2x;
                        S = 1.0f / (AB + BC + CA);
                        u = BC * S;
                        v = CA * S;
                        w = AB * S;
                        frag.texture_y = lerp(v_0.texture_uv, v_1.texture_uv, v_2.texture_uv, u, v, w);
                    }

                    // fragment perspective restore
                    perspective_restore(frag);
                    s->shader->fragment_shader(frag);
                    write_color(frag);
                }
            }
        }
        index += s->camera->x;
    }
}

inline void Renderer::alpha_test()
{
    //    cout << "alpha shading..." << endl;
}

inline void Renderer::stencil_test()
{
    //    cout << "stencil test..." << endl;
}

inline void Renderer::write_color(Fragment &frag) const
{
    int index = frag.y * s->camera->x + frag.x;
    if (frag.z < s->z_buffer[index])
    {
        s->z_buffer[index] = frag.z;
        s->frame_buffer->write_color(frag);
    }
}

inline void Renderer::dither()
{
    //    cout << "dither..." << endl;
}

void Renderer::frame_blend()
{
    //    cout << "frame blend..." << endl;
    //    for (int i = 0; i < s->camera->x * s->camera->y; i++)
    //    {
    //        s->frame_buffer->buffer[i] = s->color_buffer[i];
    //    }
}

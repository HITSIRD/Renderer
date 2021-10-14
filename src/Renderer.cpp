//
// Created by 闻永言 on 2021/8/8.
//

#include "Renderer.hpp"
#include <sstream>
#include <iomanip>
#include "iodata.hpp"
#include "Util.hpp"
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

    if (s->shadow)
    {
        cout << "shadow mapping..." << endl;
        for (auto l: s->light)
        {
            l->shadow_mapping(s->model); // calculate shadow map
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

inline bool Renderer::clipping(const Vertex &v_0, const Vertex &v_1, const Vertex &v_2)
{
    if (v_0.clip.w() < 1e-5 || v_1.clip.w() < 1e-5 || v_2.clip.w() < 1e-5) // to avoid divide by 0
    {
        return true;
    }
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

void Renderer::draw()
{
    for (auto mesh: s->model->meshes)
    {
        Uniform u = mesh->material->get_uniform();
        s->shader = mesh->material->get_shader(); // set shader for material
        s->shader->set_uniform(&u); // set uniform
        s->shader->set_light(&s->light); // set light source
        s->shader->set_camera(s->camera); // set camera

#pragma omp parallel for
        for (int i = 0; i < mesh->num_vertex; i++)
        {
            s->shader->vertex_shader(mesh->vertices[i]);
            perspective_division(mesh->vertices[i]);
            mesh->vertices[i].screen = s->camera->M_viewport * mesh->vertices[i].screen;
        }

#pragma omp parallel for
        for (int i = 0; i < mesh->num_triangle; i++)
        {
            const Vertex &v_0 = mesh->vertices[mesh->triangles[i].vertex_0];
            const Vertex &v_1 = mesh->vertices[mesh->triangles[i].vertex_1];
            const Vertex &v_2 = mesh->vertices[mesh->triangles[i].vertex_2];

            if (!clipping(v_0, v_1, v_2))
            {
                // face clipping
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
            }
        }

#pragma omp parallel for
        for (int i = 0; i < mesh->num_vertex; i++)
        {
            perspective_restore(mesh->vertices[i]); // perspective restore
        }
    }
}

void Renderer::draw_triangle(const Vertex &v_0, const Vertex &v_1, const Vertex &v_2, const float4 &normal)
{
    // real bounding box
    int min_x = max((int)min(v_0.screen.x(), min(v_1.screen.x(), v_2.screen.x())), 0);
    int min_y = max((int)min(v_0.screen.y(), min(v_1.screen.y(), v_2.screen.y())), 0);
    int max_x = min((int)max(v_0.screen.x(), max(v_1.screen.x(), v_2.screen.x())) + 1, s->camera->x - 1);
    int max_y = min((int)max(v_0.screen.y(), max(v_1.screen.y(), v_2.screen.y())) + 1, s->camera->y - 1);

    // rasterization
    for (int i = min_y; i <= max_y; i++)
    {
        int index = i * s->camera->x;
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
            if(AB > 0 && BC > 0 && CA > 0)
            {
                float u, v, w;
                if (is_in_triangle(AB, BC, CA, u, v, w))
                {
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
                        // fragment perspective restore
                        perspective_restore(frag);
                        s->shader->fragment_shader(frag);
                        write_color(frag);
                    }
                }
            }
        }
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

//
// Created by 闻永言 on 2021/7/10.
//

#include "Rasterizer.hpp"
#include "Shader.hpp"
#include <sys/time.h>
#include <iostream>

using namespace std;
using namespace Eigen;

void Rasterizer::rasterize(Mesh *mesh, vector<Fragment> &fragments)
{
    Point2f P_0, P_1, P_2;
    Point2i T_0, T_1;
    float u, v, w; // triangle core coordinate
    Point2f *point_2d = new Point2f[mesh->num_vertex];
    vector<vector<Fragment>> thread_frags; // to operate in each thread

    int max_thread = omp_get_max_threads();
//    max_thread = 4;
    omp_set_num_threads(max_thread);
    thread_frags.resize(max_thread);

#pragma omp parallel for
    // Calculate depth, back projection point_2d coordinate of every vertex
    for (int i = 0; i < mesh->num_vertex; i++)
    {
        pers_projection(mesh->vertices[i].view, point_2d[i]);
    }

#pragma omp parallel for private(P_0, P_1, P_2, T_0, T_1, u, v, w)
    for (auto triangle:mesh->triangles)
    {
        P_0 = point_2d[triangle.vertex_0];
        P_1 = point_2d[triangle.vertex_1];
        P_2 = point_2d[triangle.vertex_2];

        get_bounding_box(P_0, P_1, P_2, T_0, T_1);

        // real bounding box cut, if is after clipping, it can be removed
        if (T_0.x() > x || T_0.y() > y || T_1.x() < 0 || T_1.y() < 0)
        {
            continue;
        }
        if (T_0.x() < 0)
        {
            T_0.x() = 0;
        }
        if (T_0.y() < 0)
        {
            T_0.y() = 0;
        }
        if (T_1.x() >= x)
        {
            T_1.x() = x - 1;
        }

        if (T_1.y() >= y)
        {
            T_1.y() = y - 1;
        }

        for (int i = T_0.y(); i <= T_1.y(); i++)
        {
            for (int j = T_0.x(); j <= T_1.x(); j++)
            {
                Point2f pixel_center(float(j) + 0.5f, float(i) + 0.5f, 1);
                if (is_in_triangle(P_0, P_1, P_2, pixel_center, u, v, w))
                {
                    float current_depth = interpolate_depth(
                            mesh->vertices[triangle.vertex_0].view.z(), mesh->vertices[triangle.vertex_1].view.z(),
                            mesh->vertices[triangle.vertex_2].view.z(), u, v, w);
                    Fragment frag(j, i, current_depth);
                    frag.point =
                            w * mesh->vertices[triangle.vertex_0].world + v * mesh->vertices[triangle.vertex_1].world +
                            u * mesh->vertices[triangle.vertex_2].world;

                    if (shading_type == PHONG_SHADING)
                    {
                        frag.normal = w * mesh->vertices[triangle.vertex_0].normal +
                                      v * mesh->vertices[triangle.vertex_1].normal +
                                      u * mesh->vertices[triangle.vertex_2].normal;
                    } else
                    {
                        frag.normal = triangle.normal;
                    }
                    //                    fragments.push_back(frag);

                    int thread_id = omp_get_thread_num();
                    thread_frags[thread_id].push_back(frag);
                }
            }
        }
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);
    // merge fragments in each thread
//        int sum = 0;
//        int *frag_size = new int[max_thread];
//        int i = 0;
//        for (auto frags:thread_frags)
//        {
//            frag_size[i] = frags.size();
//            sum += frag_size[i];
//            i++;
//        }
//        fragments.resize(sum);
//        int index = 0;
//        for (int t = 0; t < max_thread ; t++)
//        {
//            for (int j = 0;j<frag_size[t];j++)
//            {
//                fragments[index] = thread_frags[t][j];
//                index++;
//            }
//        }
    for (auto frags:thread_frags)
    {
        for (auto frag:frags)
        {
            fragments.push_back(frag);
        }
    }

    gettimeofday(&end, NULL);
    double start_time = double(start.tv_usec) / 1000000.0;
    double end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;

    //    delete[] frag_size;
    delete[] point_2d;
}

void Rasterizer::pers_projection(Point3f point, Point2f &pixel)
{
    Vector3f point_2d = camera->SM * point; // screen space conversion
    pixel = point_2d / point_2d.z();
}

void Rasterizer::ortho_projection(Point3f point, Point2f &pixel)
{
    pixel = camera->OM * point;
}

void Rasterizer::get_bounding_box(Point2f p_0, Point2f p_1, Point2f p_2, Point2i &t_0, Point2i &t_1)
{
    float min_x = min(p_0.x(), min(p_1.x(), p_2.x()));
    float min_y = min(p_0.y(), min(p_1.y(), p_2.y()));
    float max_x = max(p_0.x(), max(p_1.x(), p_2.x()));
    float max_y = max(p_0.y(), max(p_1.y(), p_2.y()));

    t_0.x() = (int)min_x;
    t_0.y() = (int)min_y;
    t_1.x() = (int)max_x + 1;
    t_1.y() = (int)max_y + 1;
}

bool Rasterizer::is_in_triangle(Point2f P_0, Point2f P_1, Point2f P_2, Point2f P, float &u, float &v, float &w)
{
    Vec2 v0 = P_2 - P_0;
    Vec2 v1 = P_1 - P_0;
    Vec2 v2 = P - P_0;

    float dot_00 = v0.dot(v0);
    float dot_01 = v0.dot(v1);
    float dot_02 = v0.dot(v2);
    float dot_11 = v1.dot(v1);
    float dot_12 = v1.dot(v2);

    float deno = 1 / (dot_00 * dot_11 - dot_01 * dot_01);

    u = (dot_11 * dot_02 - dot_01 * dot_12) * deno;
    if (u < 0 || u > 1)
    {
        return false;
    }

    v = (dot_00 * dot_12 - dot_01 * dot_02) * deno;
    if (v < 0 || v > 1)
    {
        return false;
    }

    w = 1 - u - v;
    return w > 0;
}

bool Rasterizer::is_in_triangle(Point2f P_0, Point2f P_1, Point2f P_2, Point2f P)
{
    Vec2 v0 = P_2 - P_0;
    Vec2 v1 = P_1 - P_0;
    Vec2 v2 = P - P_0;

    float dot_00 = v0.dot(v0);
    float dot_01 = v0.dot(v1);
    float dot_02 = v0.dot(v2);
    float dot_11 = v1.dot(v1);
    float dot_12 = v1.dot(v2);

    float deno = 1 / (dot_00 * dot_11 - dot_01 * dot_01);

    float u = (dot_11 * dot_02 - dot_01 * dot_12) * deno;
    if (u < 0 || u > 1)
    {
        return false;
    }

    float v = (dot_00 * dot_12 - dot_01 * dot_02) * deno;
    if (v < 0 || v > 1)
    {
        return false;
    }

    float w = 1 - u - v;
    return w > 0;
}

//float Rasterizer::get_depth(Triangle triangle, Vector2f p)
//{
//    Vector3f P, cross; // world coordinate of pixel p and cross point
//    pixel_to_view(p, P);
//    get_cross_point(triangle, P, cross);
//    return cross.z();
//};

float Rasterizer::interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w)
{
    return d_0 * d_1 * d_2 / (w * d_1 * d_2 + v * d_0 * d_2 + u * d_0 * d_1);
    //    return 1.0f / (w / d_0 + v / d_1 + u / d_2);
}

//void Rasterizer::pixel_to_view(Vector2f pixel, Vector3f &view) const
//{
//    Vector3f p(pixel.x(), pixel.y(), 1);
//    view = camera->R_inv * (camera->K_inv * p * (-camera->f / 1000.0) - camera->t3);
//};
//
//void Rasterizer::get_cross_point(Triangle triangle, Vector3f P, Vector3f &cross) const
//{
//    Vector3f pc = -P;
////    Vector3f plane(triangle.A, triangle.B, triangle.C);
////    float n = (P.dot(triangle.normal) + triangle.D) / pc.dot(triangle.normal);
////    cross = -(P + n * pc);
//};

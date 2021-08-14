//
// Created by 闻永言 on 2021/7/10.
//

#include "Rasterization.hpp"
#include "omp.h"

using namespace std;
using namespace Eigen;

void Rasterization::rasterize(Mesh *mesh, Shader *shader, float *image_buffer)
{
    cout << "rasterize... " << endl;
    int real_bounding_x_min, real_bounding_y_min, real_bounding_x_max, real_bounding_y_max;
    Vector2f P_0, P_1, P_2;
    Vector2i T_0, T_1;
    float d_0, d_1, d_2; // depth of vertex in triangle
    float u, v, w; // triangle core coordinate
    Vector2f *pixel = new Vector2f[mesh->num_vertex];
    float *depth = new float[mesh->num_vertex];
    Vector3f *vertex_normal = new Vector3f[mesh->num_vertex]; // normal vector of vertex
    int count = 0;

    omp_set_num_threads(4);

#pragma omp parallel for
    // Calculate depth, back projection pixel coordinate of every vertex
    for (int i = 0; i < mesh->num_vertex; i++)
    {
        depth[i] = (camera->R * mesh->vertices[i] + camera->t).z();
        pers_projection(mesh->vertices[i], pixel[i]);
    }

    // Calculate depth, back projection pixel coordinate of every vertex
    if (shading_type == PHONG_SHADING)
    {
        shader->shadow_map(mesh);

#pragma omp parallel for
        for (uint32_t i = 0; i < mesh->num_vertex; i++)
        {
            int t = 0; // number of triangles contain the vertex
            Vector3f n(0.0f, 0.0f, 0.0f);

            if (i < DEM_x)
            {
                for (int j = 0; j < 2 * i; j++)
                {
                    if (i == mesh->triangles[j].vertex_0 || i == mesh->triangles[j].vertex_1 ||
                        i == mesh->triangles[j].vertex_2)
                    {
                        t++;
                        n.noalias() += mesh->triangles[j].normal;
                    }
                }
                vertex_normal[i] = n / t;
                continue;
            } else if (i >= (DEM_y - 1) * DEM_x)
            {
                int start = 2 * (DEM_y - 2) * (DEM_x - 1);
                for (int j = start; j < mesh->num_triangle; j++)
                {
                    if (i == mesh->triangles[j].vertex_0 || i == mesh->triangles[j].vertex_1 ||
                        i == mesh->triangles[j].vertex_2)
                    {
                        t++;
                        n.noalias() += mesh->triangles[j].normal;
                    }
                }
                vertex_normal[i] = n / t;
                continue;
            } else
            {
                int index_x = i / DEM_x;
                int start = 2 * index_x * (DEM_x - 1);
                int end = 2 * (index_x + 2) * (DEM_x - 1);
                for (int j = start; j < end; j++)
                {
                    if (i == mesh->triangles[j].vertex_0 || i == mesh->triangles[j].vertex_1 ||
                        i == mesh->triangles[j].vertex_2)
                    {
                        t++;
                        n.noalias() += mesh->triangles[j].normal;
                    }
                }
                vertex_normal[i] = n / t;
                continue;
            }
            //            for (auto triangle:mesh->triangles)
            //            {
            //                if (i == triangle.vertex_0 || i == triangle.vertex_1 || i == triangle.vertex_2)
            //                {
            //                    t++;
            //                    n.noalias() += triangle.normal;
            //                }
            //            }
        }
    }

    for (auto triangle:mesh->triangles)
    {
        P_0 = pixel[triangle.vertex_0];
        P_1 = pixel[triangle.vertex_1];
        P_2 = pixel[triangle.vertex_2];

        get_bounding_box(P_0, P_1, P_2, T_0, T_1);

        d_0 = depth[triangle.vertex_0];
        d_1 = depth[triangle.vertex_1];
        d_2 = depth[triangle.vertex_2];

        // real bounding box cut
        if (T_0.x() < 0)
        {
            real_bounding_x_min = 0;
        } else if (T_0.x() > x)
        {
            continue; // not display in image
        } else
        {
            real_bounding_x_min = T_0.x();
        }

        if (T_0.y() < 0)
        {
            real_bounding_y_min = 0;
        } else if (T_0.y() > y)
        {
            continue; // not display in image
        } else
        {
            real_bounding_y_min = T_0.y();
        }

        if (T_1.x() < 0)
        {
            continue; // not display in image
        } else if (T_1.x() >= x)
        {
            real_bounding_x_max = x - 1;
        } else
        {
            real_bounding_x_max = T_1.x();
        }

        if (T_1.y() < 0)
        {
            continue; // not display in image
        } else if (T_1.y() >= y)
        {
            real_bounding_y_max = y - 1;
        } else
        {
            real_bounding_y_max = T_1.y();
        }

        for (int i = real_bounding_y_min; i <= real_bounding_y_max; i++)
        {
            for (int j = real_bounding_x_min; j <= real_bounding_x_max; j++)
            {
                Vector2f pixel_center(float(j) + 0.5, float(i) + 0.5);
                if (is_in_triangle(P_0, P_1, P_2, pixel_center, u, v, w))
                {
                    count++;
                    //                    float current_depth = get_depth(triangle, pixel_center);

                    float current_depth = interpolate_depth(d_0, d_1, d_2, u, v, w);
                    if (current_depth < z_buffer[i * x + j])
                    {
                        z_buffer[i * x + j] = current_depth;
                        if (shading_type == FLAT_SHADING)
                        {
                            shader->flat_shading(triangle.normal, image_buffer + i * x + j);
                        } else if (shading_type == PHONG_SHADING)
                        {
                            Vector3f normal_0 = vertex_normal[triangle.vertex_0];
                            Vector3f normal_1 = vertex_normal[triangle.vertex_1];
                            Vector3f normal_2 = vertex_normal[triangle.vertex_2];
                            //                            Vector3f normal = u * normal_0 + v * normal_1 + w * normal_2;

                            shader->phong_shading(
                                    normal_0, normal_1, normal_2, u, v, w, image_buffer + i * x + j);
                            //                            shader->shadow_mapping(triangle, pixel_center, image_buffer + i * x + j);
                            shader->shadow_mapping(
                                    mesh->vertices[triangle.vertex_0], mesh->vertices[triangle.vertex_1],
                                    mesh->vertices[triangle.vertex_2], u, v, w, image_buffer + i * x + j);
                        }
                    }
                }
            }
        }
    }
    delete []pixel;
    delete []depth;

    cout << "count: " << count << endl;
}

void Rasterization::pers_projection(Vector3f point, Vector2f &pixel)
{
    Vector3f point_2f = camera->K * (camera->R * point + camera->t);
    pixel.x() = point_2f.x() / point_2f.z();
    pixel.y() = point_2f.y() / point_2f.z();
}

void Rasterization::get_bounding_box(
        Vector2f p_0, Vector2f p_1, Vector2f p_2, Vector2i &t_0, Vector2i &t_1)
{
    float min_x, max_x, min_y, max_y;
    min_x = p_0.x();
    if (p_1.x() < min_x)
    {
        min_x = p_1.x();
    }
    if (p_2.x() < min_x)
    {
        min_x = p_2.x();
    }

    min_y = p_0.y();
    if (p_1.y() < min_y)
    {
        min_y = p_1.y();
    }
    if (p_2.y() < min_y)
    {
        min_y = p_2.y();
    }

    max_x = p_0.x();
    if (p_1.x() > max_x)
    {
        max_x = p_1.x();
    }
    if (p_2.x() > max_x)
    {
        max_x = p_2.x();
    }

    max_y = p_0.y();
    if (p_1.y() > max_y)
    {
        max_y = p_1.y();
    }
    if (p_2.y() > max_y)
    {
        max_y = p_2.y();
    }

    t_0.x() = (int)min_x;
    t_0.y() = (int)min_y;
    t_1.x() = (int)max_x + 1;
    t_1.y() = (int)max_y + 1;
}

bool Rasterization::is_in_triangle(Vector2f P_0, Vector2f P_1, Vector2f P_2, Vector2f P, float &u, float &v, float &w)
{
    Vector2f v0 = P_2 - P_0;
    Vector2f v1 = P_1 - P_0;
    Vector2f v2 = P - P_0;

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
    return w >= 0;
}

//float Rasterization::get_depth(Triangle triangle, Vector2f p)
//{
//    Vector3f P, cross; // world coordinate of pixel p and cross point
//    pixel_to_view(p, P);
//    get_cross_point(triangle, P, cross);
//    return cross.z();
//};

float Rasterization::interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w)
{
    return 1.0f / (w / d_0 + v / d_1 + u / d_2);
}

//void Rasterization::pixel_to_view(Vector2f pixel, Vector3f &view) const
//{
//    Vector3f p(pixel.x(), pixel.y(), 1);
//    view = camera->R_inv * (camera->K_inv * p * (-camera->f / 1000.0) - camera->t);
//};
//
//void Rasterization::get_cross_point(Triangle triangle, Vector3f P, Vector3f &cross) const
//{
//    Vector3f pc = -P;
////    Vector3f plane(triangle.A, triangle.B, triangle.C);
////    float n = (P.dot(triangle.normal) + triangle.D) / pc.dot(triangle.normal);
////    cross = -(P + n * pc);
//};

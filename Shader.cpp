//
// Created by 闻永言 on 2021/8/7.
//

#include <iostream>
#include <fstream>
#include "Shader.hpp"
//#include "opencv2/highgui.hpp"
#include "omp.h"

using namespace Eigen;
using namespace std;

void Shader::write_depth_image(float *z_buffer)
{
    float max_depth = 1000.0, min_depth = 0;
    cout << "rendering..." << endl;
    string file = "data/depth_image.png";

    int x = light_view->pixel_x;
    int y = light_view->pixel_y;

    // Gamma correction
    float max_l = 0.0, min_l = max_depth;
    for (int i = 0; i < x * y; i++)
    {
        if (z_buffer[i] > max_l && (z_buffer[i] < max_depth - 1))
        {
            max_l = z_buffer[i];
        }
        if (z_buffer[i] < min_l)
        {
            min_l = z_buffer[i];
        }
    }

    max_l = min(max_l, max_depth);
    min_l = max(min_l, min_depth);
    float dynamic = max_l - min_l;

    for (int i = 0; i < x * y; i++)
    {
        if (fabs(z_buffer[i] - max_depth) < 0.1) // make background be black
        {
            z_buffer[i] = 1.0;
            continue;
        }
        if (z_buffer[i] < 0.0)
        {
            z_buffer[i] = 0.0;
            continue;
        }
        z_buffer[i] = (z_buffer[i] - min_l) / dynamic; // normalization
    }

    uint16_t lut[65536];

    for (int i = 0; i < 65535; i++)
    {
        lut[i] = (uint16_t)(pow(float(i) / 65535.0, 1 / 2.2) * 65535.0); // 16 bit gray
    }

    cv::Mat image = cv::Mat::zeros(x, y, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < x * y; i++)
    {
        *p = 65535 - lut[(uint16_t)(z_buffer[i] * 65535)];
        p++;
    }
    cv::imwrite(file, image);
}

void get_bounding_box(
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

bool is_in_triangle(Vector2f P_0, Vector2f P_1, Vector2f P_2, Vector2f P, float &u, float &v, float &w)
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

float interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w)
{
    return 1.0f / (w / d_0 + v / d_1 + u / d_2);
}

//void Shader::pixel_to_view(Vector2f pixel, Vector3f &view) const
//{
//    Vector3f p(pixel.x(), pixel.y(), 1);
//    view = camera->R_inv * (camera->K_inv * p * (-camera->f / 1000.0) - camera->t);
//};
//
//void Shader::get_cross_point(Triangle triangle, Vector3f P, Vector3f &cross)
//{
//    Vector3f pc = camera->center - P;
//    Vector3f normal(triangle.A, triangle.B, triangle.C);
//    float n = (triangle.plane_point - camera->center).dot(normal) / pc.dot(normal);
//    cross = camera->center + n * pc;
//};

//float Shader::get_depth(Triangle triangle, Vector2f p, Vector3f &cross)
//{
//    Vector3f P; // world coordinate of pixel p and cross point
//    pixel_to_view(p, P);
//    //    cout << "Point3f: " << P << endl;
//    get_cross_point(triangle, P, cross);
//    //    cout << "cross: " << cross << endl;
//    return (light_view->R * cross + light_view->t).z();
//}

void Shader::read_config(string &config_file)
{
    ifstream in;
    in.open(config_file.c_str());

    float norm_x, norm_y, norm_z;
    in >> norm_x >> norm_y >> norm_z;
    cout << "light normal: " << norm_x << " " << norm_y << " " << norm_z << endl;
    light << norm_x, norm_y, norm_z;
    light /= light.norm(); // normalized

    in >> ka >> kd >> spec_rank;
    ks = 1 - ka - kd;
    cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
    cout << "specular rank: " << spec_rank << endl;

    float ccd_size_x, ccd_size_y, f;
    int pixel_x, pixel_y;
    float x, y, z, h, p, b;

    in >> pixel_x >> pixel_y >> ccd_size_x >> ccd_size_y >> f;
    in >> x;
    in >> y;
    in >> z;
    in >> h;
    in >> p;
    in >> b;

    in.close();

    light_view = new Camera();
    light_view->read_param(pixel_x, pixel_y, ccd_size_x, ccd_size_y, f, x, y, z, h, p, b);
    light_view->calculate();

    view << 0, 0, -1.0;
    calibrate();
}

void Shader::calibrate()
{
    //    light = camera->R * light;
    //    light << 0, 0, 1;
    view = camera->R_inv * view;
}

void Shader::flat_shading(Vector3f normal, float *point)
{
    float lum = normal.dot(-light);
    //    float lum = abs(normal.dot(light) / (normal.norm() * light.norm()));
    float diffuse = max(lum, 0.0f);

    // Angle between reflected light and viewing
    Vector3f bisector = ((-light) + view) / 2.0f;
    //    float specular = pow(max(normal.dot(bisector) / (normal.norm() * bisector.norm()), 0.0), spec_rank);
    float specular = pow(max(normal.dot(bisector), 0.0f), spec_rank);
    *point = ka + kd * diffuse + ks * specular;
}

void Shader::phong_shading(
        Vector3f &normal_0, Vector3f &normal_1, Vector3f &normal_2, float u, float v, float w, float *point)
{
    // normal vector interpolation
    Vector3f normal = w * normal_0 + v * normal_1 + u * normal_2;
    float lum = normal.dot(-light);
    float diffuse = max(lum, 0.0f);
    //    float diffuse = max(abs(lum), 0.0);

    // Angle between reflected light and viewing
    Vector3f bisector = ((-light) + view) / 2.0f;
    float specular = pow(max(normal.dot(bisector), 0.0f), spec_rank);
    //    float specular = pow(max(abs(normal.dot(bisector) / (normal.norm() * bisector.norm())), 0.0), spec_rank);
    *point = ka + kd * diffuse + ks * specular;
}

//void Shader::phong_shading(Vector3f &normal, float *point)
//{
//    // normal vector interpolation
//    float lum = normal.dot(-light);
//    float diffuse = max(lum, 0.0f);
//
//    // Angle between reflected light and viewing
//    Vector3f bisector = ((-light) + view) / 2.0f;
//    float specular = pow(max(normal.dot(bisector), 0.0f), spec_rank);
//    *point = ka + kd * diffuse + ks * specular;
//}

//void Shader::shadow_mapping(Triangle triangle, Vector2f pixel, float *point)
//{
//    Vector3f cross; // cross point
//    Vector2f shadow_map_index;
//
//    //    cout << "pixel: " << pixel << endl;
//    float depth = get_depth(triangle, pixel, cross);
//    back_projection(cross, shadow_map_index);
//    //    int index_x = max((int)shadow_map_index.x(), 0);
//    //    int index_y = max((int)shadow_map_index.y(), 0);
//    int index_x = (int)shadow_map_index.x();
//    int index_y = (int)shadow_map_index.y();
//    if (index_x >= light_view->pixel_x || index_y >= light_view->pixel_x || index_x < 0 || index_y < 0)
//    {
////        cout << "cross: " << cross << endl;
////        cout << "shadow: " << shadow_map_index << endl;
//        return;
//    }
//
//    float deviation = abs(depth - shadow_buffer[index_y * light_view->pixel_x + index_x]);
//    if (deviation > EPSILON)
//    {
//        //        cout << deviation << endl;
//        *point = 0.005;
//    }
//}

void Shader::shadow_mapping(
        Eigen::Vector3f &vertex_0, Eigen::Vector3f &vertex_1, Eigen::Vector3f &vertex_2, float u, float v, float w,
        float *point)
{
    Vector2f shadow_map_index;
    Vector3f p = w * vertex_0 + v * vertex_1 + u * vertex_2;
    ortho_projection(p, shadow_map_index);
    float depth = (light_view->R * p + light_view->t).z();

    int index_x = (int)shadow_map_index.x();
    int index_y = (int)shadow_map_index.y();
    if (index_x >= light_view->pixel_x || index_y >= light_view->pixel_x || index_x < 0 || index_y < 0)
    {
        return;
    }
    float deviation = abs(depth - shadow_buffer[index_y * light_view->pixel_x + index_x]);
    if (deviation > EPSILON)
    {
        //        cout << deviation << endl;
        *point = 0.005;
    }
}

void Shader::shadow_map(Mesh *mesh)
{
    int real_bounding_x_min, real_bounding_y_min, real_bounding_x_max, real_bounding_y_max;
    Vector2f P_0, P_1, P_2;
    Vector2i T_0, T_1;
    float d_0, d_1, d_2; // depth of vertex in triangle
    float u, v, w; // triangle core coordinate
    Vector2f *pixel = new Vector2f[mesh->num_vertex];
    float *depth = new float[mesh->num_vertex];
    int x = light_view->pixel_x;
    int y = light_view->pixel_y;
    int count = 0;

    omp_set_num_threads(4);

    K << (float)(light_view->pixel_x / 2) / 160.0f, 0, 0, 0, float(light_view->pixel_y / 2) / 160.0f, 0, 0, 0, 1;
    offset << float(light_view->pixel_x / 2), float(light_view->pixel_y / 2), 0.0f;

    // Calculate depth, back projection pixel coordinate of every vertex
#pragma omp parallel for
    for (int i = 0; i < mesh->num_vertex; i++)
    {
        depth[i] = (light_view->R * mesh->vertices[i] + light_view->t).z();
        ortho_projection(mesh->vertices[i], pixel[i]);
    }

    shadow_buffer = new float[x * y];
    for (int i = 0; i < x * y; i++)
    {
        shadow_buffer[i] = max_depth;
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
                    if (current_depth < shadow_buffer[i * x + j])
                    {
                        shadow_buffer[i * x + j] = current_depth;
                    }
                }
            }
        }
    }

//    write_depth_image(shadow_buffer);
    delete[]pixel;
    delete[]depth;
}

void Shader::pers_projection(Vector3f point, Vector2f &pixel)
{
    Vector3f point_2f = light_view->K * (light_view->R * point + light_view->t);
    pixel.x() = point_2f.x() / point_2f.z();;
    pixel.y() = point_2f.y() / point_2f.z();
}

void Shader::ortho_projection(Vector3f point, Vector2f &pixel)
{
    //    Vector3f z_offset(0.0f, 0.0f, 0.0f);
    Vector3f point_2f = K * (light_view->R * point + light_view->t) + offset;
    pixel.x() = point_2f.x();
    pixel.y() = point_2f.y();
}

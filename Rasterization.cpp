//
// Created by 闻永言 on 2021/7/10.
//

#include "opencv2/highgui.hpp"
#include "Rasterization.hpp"
#include <fstream>
#include <algorithm>

using namespace std;
using namespace Eigen;

void Rasterization::read_data(string &dem_file, string &camera_file)
{
    cout << "read data... " << endl;
    Convert *con = new Convert();
    camera = con->calibrate(camera_file);
    iodata::data *d = new iodata::data();
    d->read_DEM(dem_file);
    mesh = d->dem2mesh();
    delete con;
    delete d;
};

void Rasterization::initialize()
{
    cout << "initialize..." << endl;
    x = camera->pixel_x;
    y = camera->pixel_y;
    z_buffer = new double[x * y];
    image_buffer = new double[x * y];

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            z_buffer[i * x + j] = max_depth;
        }
    }

    //        if (v_2.y() > v_1.y())
    //        {
    //            swap(v_2, v_1);
    //        }
    //        if (v_1.y() > v_0.y())
    //        {
    //            swap(v_1, v_0);
    //        }
    //        if (v_2.y() > v_1.y())
    //        {
    //            swap(v_2, v_1);
    //        }

    //    sort_vertex();

    // calculate the lut
    for (int i = 0; i < 65535; i++)
    {
        lut[i] = (uint16_t)(pow(double(i) / 65535.0, GAMMA) * 65535.0); // 16 bit gray
    }
};

void Rasterization::rasterize(Shader *shader)
{
    cout << "rasterize... " << endl;
    int real_bounding_x_min, real_bounding_y_min, real_bounding_x_max, real_bounding_y_max;
    Vector2d P_0, P_1, P_2;
    Vector2i T_0, T_1;
    double d_0, d_1, d_2; // depth of vertex in triangle
    double u, v, w; // triangle core coordinate
    double *depth = new double[mesh.num_vertex];
    int count = 0;

    // Calculate depth of every vertex
    for (int i = 0; i < mesh.num_vertex; i++)
    {
        depth[i] = (camera->R * mesh.vertices[i] + camera->t).z();
    }

    for (auto triangle:mesh.triangles)
    {
        back_projection(mesh.vertices[triangle.vertex_0], P_0);
        back_projection(mesh.vertices[triangle.vertex_1], P_1);
        back_projection(mesh.vertices[triangle.vertex_2], P_2);

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
                Vector2d pixel_center(double(j) + 0.5, double(i) + 0.5);
                //                count++;
                if (is_in_triangle(P_0, P_1, P_2, pixel_center, u, v, w))
                {
                    count++;
                    //                    double current_depth = get_depth(*triangle, pixel_center);

                    double current_depth = interpolate_depth(d_0, d_1, d_2, u, v, w);
                    if (current_depth < z_buffer[i * x + j])
                    {
                        z_buffer[i * x + j] = current_depth;
                        shader->shader(triangle.normal, image_buffer + i * x + j);
                    }
                }
            }
        }
    }
    cout << "count: " << count << endl;
};

void Rasterization::sort_vertex()
{
    //    for (auto triangle:triangles)
    //    {
    //        //        Vertex A_0 = triangle->vertex_0;
    //        //        Vertex A_1 = triangle->vertex_1;
    //        //        Vertex A_2 = triangle->vertex_2;
    //        //        Vector3f A_01(A_1.x - A_0.x, A_1.y - A_0.y, A_1.z - A_0.z);
    //        //        Vector3f A_02(A_2.x - A_0.x, A_2.y - A_0.y, A_2.z - A_0.z);
    //
    //        Vector3f A_01(
    //                triangle->vertex_1.x() - triangle->vertex_0.x(), triangle->vertex_1.y() - triangle->vertex_0.y(),
    //                triangle->vertex_1.z() - triangle->vertex_0.z());
    //        Vector3f A_02(
    //                triangle->vertex_2.x() - triangle->vertex_0.x(), triangle->vertex_2.y() - triangle->vertex_0.y(),
    //                triangle->vertex_2.z() - triangle->vertex_0.z());
    //
    //        if (A_01.cross(A_02).lpNorm<1>() < 0)
    //        {
    //            swap(triangle->vertex_1, triangle->vertex_2);
    //        }
    //    }
};

void Rasterization::back_projection(Vector3d point, Vector2d &pixel)
{
    Vector3d point_2d = camera->K * (camera->R * point + camera->t);
    pixel.x() = point_2d.x() / point_2d.z();
    pixel.y() = point_2d.y() / point_2d.z();
};

void Rasterization::get_bounding_box(
        Vector2d p_0, Vector2d p_1, Vector2d p_2, Vector2i &t_0, Vector2i &t_1)
{
    double min_x, max_x, min_y, max_y;
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
};

bool
Rasterization::is_in_triangle(Vector2d P_0, Vector2d P_1, Vector2d P_2, Vector2d P, double &u, double &v, double &w)
{
    Vector2d v0 = P_2 - P_0;
    Vector2d v1 = P_1 - P_0;
    Vector2d v2 = P - P_0;

    double dot_00 = v0.dot(v0);
    double dot_01 = v0.dot(v1);
    double dot_02 = v0.dot(v2);
    double dot_11 = v1.dot(v1);
    double dot_12 = v1.dot(v2);

    double deno = 1 / (dot_00 * dot_11 - dot_01 * dot_01);

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

    //    Vector2d v01 = P_1 - P_0;
    //    Vector2d v12 = P_2 - P_1;
    //    Vector2d v20 = P_0 - P_2;
    //    Vector2d v0P = P - P_0;
    //    Vector2d v1P = P - P_1;
    //    Vector2d v2P = P - P_2;
    //
    //    double c1 = v01.x() * v0P.y() - v0P.x() * v01.y();
    //    double c2 = v12.x() * v1P.y() - v1P.x() * v12.y();
    //    double c3 = v20.x() * v2P.y() - v2P.x() * v20.y();
    //
    //    //    double v01_x = P_1.x() - P_0.x();
    //    //    double v12_x = P_2.x() - P_1.x();
    //    //    double v20_x = P_0.x() - P_2.x();
    //    //    double v0P_x = P.x() - P_0.x();
    //    //    double v1P_x = P.x() - P_1.x();
    //    //    double v2P_x = P.x() - P_2.x();
    //    //
    //    //    double v01_y = P_1.y() - P_0.y();
    //    //    double v12_y = P_2.y() - P_1.y();
    //    //    double v20_y = P_0.y() - P_2.y();
    //    //    double v0P_y = P.y() - P_0.y();
    //    //    double v1P_y = P.y() - P_1.y();
    //    //    double v2P_y = P.y() - P_2.y();
    //    //
    //    //    double c1 = v01_x * v0P_y - v0P_x * v01_y;
    //    //    double c2 = v12_x * v1P_y - v1P_x * v12_y;
    //    //    double c3 = v20_x * v2P_y - v2P_x * v20_y;
    //
    //    if (c1 > 0)
    //    {
    //        if (c2 < 0)
    //        {
    //            return false;
    //        } else
    //        {
    //            return c3 > 0;
    //        }
    //    } else
    //    {
    //        if (c2 > 0)
    //        {
    //            return false;
    //        } else
    //        {
    //            return c3 < 0;
    //        }
    //    }
};

double Rasterization::get_depth(Triangle triangle, Vector2d p)
{
    Vector3d P, cross; // world coordinate of pixel p and cross point
    pixel_to_view(p, P);
    get_cross_point(triangle, P, cross);
    return cross.z();
};

double Rasterization::interpolate_depth(double d_0, double d_1, double d_2, double u, double v, double w)
{
    return 1.0 / (u / d_0 + v / d_1 + w / d_2);
};

void Rasterization::pixel_to_view(Vector2d pixel, Vector3d &view) const
{
    Vector3d p(pixel.x(), pixel.y(), 1);
    view = camera->K_inv * p * (-camera->f / 1000.0);
};

void Rasterization::get_cross_point(Triangle triangle, Vector3d P, Vector3d &cross) const
{
    Vector3d pc = -P;
    Vector3d plane(triangle.A, triangle.B, triangle.C);
    double n = (P.dot(plane) + triangle.D) / pc.dot(plane);
    cross = -(P + n * pc);
};

void Rasterization::write_depth_image()
{
    cout << "rendering..." << endl;
    string file = "data/depth_image.png";

    // Gamma correction
    double max_l = 0.0, min_l = max_depth;
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
    double dynamic = max_l - min_l;

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

    image = cv::Mat::zeros(x, y, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < x * y; i++)
    {
        *p = 65536 - lut[(uint16_t)(z_buffer[i] * 65536)];
        p++;
    }
};

void Rasterization::write_result_image()
{
    cout << "rendering..." << endl;
    string file = "data/image.png";

    image = cv::Mat::zeros(y, x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < x * y; i++)
    {
        *p = lut[(uint16_t)(image_buffer[i] * 65535)];
        p++;
    }
    cv::imwrite(file, image);
};

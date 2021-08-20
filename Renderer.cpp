//
// Created by 闻永言 on 2021/8/8.
//

#include "Renderer.hpp"
#include "Convert.hpp"
#include "iodata.hpp"
#include <iostream>
#include <fstream>
#include <sys/time.h>
//#include <omp.h>

using namespace std;
using namespace Eigen;

void Renderer::read_data(string &dem_file, string &camera_file)
{
    cout << "read data... " << endl;
    mesh = new Mesh();
    Convert *con = new Convert();
    camera = con->calibrate(camera_file);
    iodata *d = new iodata();
    d->read_DEM(dem_file);
    d->dem2mesh(*mesh);
    x = camera->pixel_x;
    y = camera->pixel_y;
    delete con;
    delete d;
}

void Renderer::read_config(string &config_file, int type)
{
    shading_type = type;
    float ka, kd, ks, spec_rank;
    ifstream in;
    in.open(config_file.c_str());

    float norm_x, norm_y, norm_z;
    in >> norm_x >> norm_y >> norm_z;
    cout << "light normal: " << norm_x << " " << norm_y << " " << norm_z << endl;

    Vector4f light(norm_x, norm_y, norm_z, 0);
    light.normalize(); // normalized
    light = camera->R * light; // transform into view space

    in >> ka >> kd >> spec_rank;
    ks = 1 - ka - kd;
    cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
    cout << "specular rank: " << spec_rank << endl;

    float ccd_size_x, ccd_size_y, f;
    int shadow_x, shadow_y;
    float lx, ly, lz, lh, lp, lb;

    in >> shadow_x >> shadow_y >> ccd_size_x >> ccd_size_y >> f;
    in >> lx;
    in >> ly;
    in >> lz;
    in >> lh;
    in >> lp;
    in >> lb;

    in.close();

    light_view = new Camera();
    light_view->read_param(shadow_x, shadow_y, ccd_size_x, ccd_size_y, f, lx, ly, lz, lh, lp, lb);
    light_view->convert();
    in.close();

    vertex_shader = new VertexShader(ka, kd, ks, spec_rank, light);
    fragment_shader = new FragmentShader(ka, kd, ks, spec_rank, light);
    initialize();
}

void Renderer::initialize()
{
    cout << "initialize..." << endl;
    raster = new Rasterizer(x, y, camera, shading_type, max_depth);
    stencil_buffer = new float[x * y];
    z_buffer = new float[x * y];
    frame_buffer = new float[x * y];

    max_threads = omp_get_max_threads();
    cout << "max_threads: " << max_threads << endl;

    for (int i = 0; i < x * y; i++)
    {
        stencil_buffer[i] = 0;
        z_buffer[i] = max_depth;
    }
    //    sort_vertex();

    // convert the lut
    for (int i = 0; i < 65535; i++)
    {
        lut[i] = (uint16_t)(pow(float(i) / 65535.0f, GAMMA) * 65535.0f); // 16 bit gray
    }

    // Calculate depth, back projection pixel coordinate of every vertex
    if (shading_type == PHONG_SHADING)
    {
        omp_set_num_threads(max_threads);
#pragma omp parallel for
        for (uint32_t i = 0; i < mesh->num_vertex; i++)
        {
            int t = 0; // number of triangles contain the vertex
            Vec3 n(0, 0, 0, 0);

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
                mesh->vertices[i].normal = n / t;
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
                mesh->vertices[i].normal = n / t;
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
                mesh->vertices[i].normal = n / t;
                continue;
            }
            //            for (auto triangle:mesh->triangles)
            //            {
            //                if (i == triangle.vertex_0 || i == triangle.vertex_1 || i == triangle.vertex_2)
            //                {
            //                    t3++;
            //                    n.noalias() += triangle.normal;
            //                }
            //            }
        }
    }

    shadow_mapping(); // calculate shadow map
}

void Renderer::sort_vertex()
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
}

void Renderer::render()
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    view_transform();
    gettimeofday(&end, NULL);
    double start_time = double(start.tv_usec) / 1000000.0;
    double end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;

    vertex_shading();

    clipping();

    gettimeofday(&start, NULL);
    rasterize();
    gettimeofday(&end, NULL);
    start_time = double(start.tv_usec) / 1000000.0;
    end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;

    gettimeofday(&start, NULL);
    fragment_shading();
    gettimeofday(&end, NULL);
    start_time = double(start.tv_usec) / 1000000.0;
    end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;

    alpha_test();

    gettimeofday(&start, NULL);
    stencil_test();
    gettimeofday(&end, NULL);
    start_time = double(start.tv_usec) / 1000000.0;
    end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;

    gettimeofday(&start, NULL);
    depth_test();
    gettimeofday(&end, NULL);
    start_time = double(start.tv_usec) / 1000000.0;
    end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;

    dither();

    gettimeofday(&start, NULL);
    frame_blend();
    gettimeofday(&end, NULL);
    start_time = double(start.tv_usec) / 1000000.0;
    end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
    cout << "cost time: " << end_time - start_time << endl;
}

void Renderer::view_transform()
{
    cout << "view transform..." << endl;

    omp_set_num_threads(max_threads);
    #pragma omp parallel for
    for (int i = 0; i < mesh->num_vertex; i++)
    {
        mesh->vertices[i].view = camera->VM * mesh->vertices[i].world;
        mesh->vertices[i].normal = camera->R * mesh->vertices[i].normal; // normal vector transform
    }
    #pragma omp parallel for
    for (int i = 0; i < mesh->num_triangle; i++)
    {
        mesh->triangles[i].normal = camera->R * mesh->triangles[i].normal; // normal vector transform
    }
}

void Renderer::vertex_shading()
{
    cout << "vertex shading..." << endl;
    //    for (int i = 0; i < mesh->num_vertex; i++)
    //    {
    //        vertex_shader->shading(mesh->vertices[i], shading_type);
    //    }
}

void Renderer::clipping()
{
    cout << "clipping..." << endl;
}

void Renderer::rasterize()
{
    cout << "rasterize..." << endl;
    raster->rasterize(mesh, frag_buffer);

    // initialize the test buffer
    num_frag = frag_buffer.size();
    cout << "num_frag: " << num_frag << endl;
    test_buffer = new bool[num_frag];
    for (int i = 0; i < num_frag; i++)
    {
        test_buffer[i] = true;
    }
}

void Renderer::fragment_shading()
{
    cout << "fragment shading..." << endl;

        omp_set_num_threads(max_threads);
    #pragma omp parallel for
    for (int i = 0; i < num_frag; i++)
    {
        fragment_shader->shading(frag_buffer[i], shading_type);
        shadow(frag_buffer[i]);
    }
}

void Renderer::alpha_test()
{
    cout << "alpha shading..." << endl;
}

void Renderer::stencil_test()
{
    cout << "stencil test..." << endl;

    //    omp_set_num_threads(max_threads);
    //#pragma omp parallel for
    for (int i = 0; i < num_frag; i++)
    {
        int index = frag_buffer[i].y * x + frag_buffer[i].x;
        if (stencil_buffer[index] != 0)
        {
            test_buffer[i] = false;
        }
    }
}

void Renderer::depth_test()
{
    cout << "depth test..." << endl;

    for (int i = 0; i < num_frag; i++)
    {
        if (test_buffer[i])
        {
            int index = frag_buffer[i].y * x + frag_buffer[i].x;
            if (frag_buffer[i].z < z_buffer[index])
            {
                z_buffer[index] = frag_buffer[i].z;
            } else
            {
                test_buffer[i] = false;
            }
        }
    }
}

void Renderer::dither()
{
    cout << "dither..." << endl;
}

void Renderer::frame_blend()
{
    cout << "frame blend..." << endl;

    for (int i = 0; i < num_frag; i++)
    {
        if (test_buffer[i])
        {
            int index = frag_buffer[i].y * x + frag_buffer[i].x;
            frame_buffer[index] = frag_buffer[i].color;
        }
    }
}

uint32_t Renderer::num_vertex()
{
    return mesh->num_vertex;
}

uint32_t Renderer::num_triangle()
{
    return mesh->num_triangle;
}

void Renderer::pers_projection(Camera *c, Point3f point, Point2f &pixel)
{
    Point2f point_2d = c->SM * point; // screen space conversion
    pixel = point_2d / point_2d.z();
}

void Renderer::ortho_projection(Camera *c, Point3f point, Point2f &pixel)
{
    pixel = c->OM * point;
    pixel.z() = 1;
}

void Renderer::shadow_mapping()
{
    shadow_map = new float[light_view->pixel_x * light_view->pixel_y];
    for (int i = 0; i < light_view->pixel_x * light_view->pixel_y; i++)
    {
        shadow_map[i] = max_depth;
    }

    Point2f P_0, P_1, P_2;
    Point2i T_0, T_1;
    float u, v, w; // triangle core coordinate
    auto *point_3d_view = new Point3f[mesh->num_vertex];
    auto *point_2d = new Point2f[mesh->num_vertex];

    //    omp_set_num_threads(4);
    // Calculate depth, back projection point_2d coordinate of every vertex
    //#pragma omp parallel for
    for (int i = 0; i < mesh->num_vertex; i++)
    {
        point_3d_view[i] = light_view->VM * mesh->vertices[i].world;
        ortho_projection(light_view, point_3d_view[i], point_2d[i]);
    }

    //#pragma omp parallel for private(real_bounding_x_min, real_bounding_y_min, real_bounding_x_max, real_bounding_y_max, P_0, P_1, P_2, T_0, T_1, u, v, w)
    for (const auto &triangle:mesh->triangles)
    {
        P_0 = point_2d[triangle.vertex_0];
        P_1 = point_2d[triangle.vertex_1];
        P_2 = point_2d[triangle.vertex_2];

        get_bounding_box(P_0, P_1, P_2, T_0, T_1);

        // real bounding box cut, if is after clipping, it can be removed
        if (T_0.x() > light_view->pixel_x || T_0.y() > light_view->pixel_y || T_1.x() < 0 || T_1.y() < 0)
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
        if (T_1.x() >= light_view->pixel_x)
        {
            T_1.x() = light_view->pixel_x - 1;
        }

        if (T_1.y() >= light_view->pixel_y)
        {
            T_1.y() = light_view->pixel_y - 1;
        }

        for (int i = T_0.y(); i <= T_1.y(); i++)
        {
            for (int j = T_0.x(); j <= T_1.x(); j++)
            {
                Point2f pixel_center(float(j) + 0.5f, float(i) + 0.5f, 1);
                if (is_in_triangle(P_0, P_1, P_2, pixel_center, u, v, w))
                {
                    float depth = interpolate_depth(
                            point_3d_view[triangle.vertex_0].z(), point_3d_view[triangle.vertex_1].z(),
                            point_3d_view[triangle.vertex_2].z(), u, v, w);
                    if (depth < shadow_map[i * light_view->pixel_x + j])
                    {
                        shadow_map[i * light_view->pixel_x + j] = depth;
                    }
                }
            }
        }
    }

    //    write_depth_image(shadow_map, light_view);
    delete[] point_2d;
    delete[] point_3d_view;
}

void Renderer::shadow(Fragment &frag)
{
    Point2f shadow_map_index;
    Point3f view = light_view->VM * frag.point;
    ortho_projection(light_view, view, shadow_map_index);
    int index_x = (int)shadow_map_index.x();
    int index_y = (int)shadow_map_index.y();
    if (index_x >= light_view->pixel_x || index_y >= light_view->pixel_x || index_x < 0 || index_y < 0)
    {
        return;
    }

    float depth = view.z();
    float deviation = abs(depth - shadow_map[index_y * light_view->pixel_x + index_x]);
    if (deviation > EPSILON)
    {
        frag.color = 0.005;
    }
}

void Renderer::get_bounding_box(Point2f p_0, Point2f p_1, Point2f p_2, Point2i &t_0, Point2i &t_1)
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

bool Renderer::is_in_triangle(
        const Point2f &P_0, const Point2f &P_1, const Point2f &P_2, const Point2f &P, float &u, float &v, float &w)
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

float Renderer::interpolate_depth(float d_0, float d_1, float d_2, float u, float v, float w)
{
    return 1.0f / (w / d_0 + v / d_1 + u / d_2);
}

void Renderer::write_depth_image(float *depth_buffer, Camera *c)
{
    cout << "rendering..." << endl;
    string file = "data/depth_image.png";

    // Gamma correction
    float max_l = 0.0, min_l = max_depth;
    for (int i = 0; i < c->pixel_y * c->pixel_x; i++)
    {
        if (depth_buffer[i] > max_l && (depth_buffer[i] < max_depth - 1))
        {
            max_l = depth_buffer[i];
        }
        if (depth_buffer[i] < min_l)
        {
            min_l = depth_buffer[i];
        }
    }

    max_l = min(max_l, max_depth);
    min_l = max(min_l, min_depth);
    float dynamic = max_l - min_l;

    for (int i = 0; i < c->pixel_y * c->pixel_x; i++)
    {
        if (fabs(depth_buffer[i] - max_depth) < 0.1) // make background be black
        {
            depth_buffer[i] = 1.0;
            continue;
        }
        if (depth_buffer[i] < 0.0)
        {
            depth_buffer[i] = 0.0;
            continue;
        }
        depth_buffer[i] = (depth_buffer[i] - min_l) / dynamic; // normalization
    }

    image = cv::Mat::zeros(c->pixel_y, c->pixel_x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < c->pixel_y * c->pixel_x; i++)
    {
        *p = 65535 - lut[(uint16_t)(depth_buffer[i] * 65535)];
        p++;
    }
    cv::imwrite(file, image);
}

void Renderer::write_result_image()
{
    cout << "rendering..." << endl;
    string file = "data/image.png";

    image = cv::Mat::zeros(y, x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < x * y; i++)
    {
        *p = lut[(uint16_t)(frame_buffer[i] * 65535)];
        p++;
    }
    cv::imwrite(file, image);
}

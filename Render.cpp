//
// Created by 闻永言 on 2021/8/8.
//

#include "Render.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace Eigen;

void Render::read_data(string &dem_file, string &camera_file)
{
    cout << "read data... " << endl;
    Convert *con = new Convert();
    camera = con->calibrate(camera_file);
    iodata::data *d = new iodata::data();
    d->read_DEM(dem_file);
    mesh = d->dem2mesh();
    shader = new Shader(camera);
    DEM_x = d->x;
    DEM_y = d->y;
    delete con;
    delete d;
}

void Render::read_config(string &config_file){
    shader->read_config(config_file);
}

void Render::initialize(int shader_)
{
    cout << "initialize..." << endl;
    x = camera->pixel_x;
    y = camera->pixel_y;
    image_buffer = new float[x * y];
    shading_type = shader_;
    raster = new Rasterization(x, y, DEM_x, DEM_y, camera, max_depth, shading_type);

    //    sort_vertex();

    // calculate the lut
    for (int i = 0; i < 65535; i++)
    {
        lut[i] = (uint16_t)(pow(float(i) / 65535.0f, GAMMA) * 65535.0f); // 16 bit gray
    }
}

void Render::rasterize(){
    raster->rasterize(mesh, shader, image_buffer);
}

uint32_t Render::num_vertex()
{
    return mesh->num_vertex;
}

uint32_t Render::num_triangle()
{
    return mesh->num_triangle;
}

void Render::sort_vertex()
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

void Render::write_depth_image(float *z_buffer)
{
    cout << "rendering..." << endl;
    string file = "data/depth_image.png";

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

    image = cv::Mat::zeros(x, y, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < x * y; i++)
    {
        *p = 65535 - lut[(uint16_t)(z_buffer[i] * 65535)];
        p++;
    }
    cv::imwrite(file, image);
}

void Render::write_result_image()
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
}

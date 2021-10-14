//
// Created by 闻永言 on 2021/7/10.
//

#include "iodata.hpp"
#include <iostream>
#include <fstream>
#include "FlatMaterial.hpp"
#include "PhongMaterial.hpp"

using namespace std;

void iodata::read_DEM(const string &file_name, Model *model)
{
    string DEM_file;
    int x; // DEM resolution
    int y; // DEM resolution
    float offset_x; // make meshes at center
    float offset_y; // make meshes at center
    float sample; // distance between two sample world

    ifstream in;
    in.open(file_name.c_str());
    in >> DEM_file;
    in.close();

    in.open(DEM_file.c_str()); // load DEM file
    in >> x >> y >> sample;
    cout << "DEM_x = " << x << ", DEM_y = " << y << ", sample = " << sample << endl;

    auto DEM = new float[x * y];

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            {
                in >> DEM[i * x + j];
            }
        }
    }
    in.close();

    offset_x = (float)(x - 1) * sample / 2.0f;
    offset_y = (float)(y - 1) * sample / 2.0f;

    auto *mesh = new Mesh();
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            float4 c(float(i) * sample - offset_x, float(j) * sample - offset_y, DEM[i * x + j], 1.0f);
            Vertex v(c);
            mesh->add_vertex(v);
        }
    }

    for (int i = 0; i < y - 1; i++)
    {
        for (int j = 0; j < x - 1; j++)
        {
            Triangle triangle_0(mesh->vertices, i * x + j, i * x + x + j, i * x + j + 1);
            Triangle triangle_1(mesh->vertices, i * x + j + 1, i * x + x + j, i * x + x + j + 1);
            mesh->add_triangle(triangle_0);
            mesh->add_triangle(triangle_1);
        }
    }

    cout << "vertex number: " << mesh->num_vertex << endl;
    cout << "triangles number: " << mesh->num_triangle << endl;

    // load material source parameters
    in.open(file_name.c_str());
    in >> DEM_file;

    string type;
    float ka, kd, ks, spec_rank;
    in >> type;
    in >> ka >> kd >> ks >> spec_rank;
    cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
    cout << "specular rank: " << spec_rank << endl;
    if (type == "FLAT")
    {
        Material *m = new FlatMaterial();
        m->ambient = ka;
        m->diffuse = kd;
        m->specular = ks;
        m->spec_rank = spec_rank;
        mesh->material = m;
    } else if (type == "PHONG")
    {
        Material *m = new PhongMaterial();
        m->ambient = ka;
        m->diffuse = kd;
        m->specular = ks;
        m->spec_rank = spec_rank;
        mesh->material = m;
    } else
    {
        cerr << "MATERIAL TYPE ERROR" << endl;
        throw exception();
    }

    in.close();
    delete[] DEM;

    model->meshes.push_back(mesh);
}

State *iodata::load_config(const string &config)
{
    auto *s = new State();
    cout << config << endl;

    ifstream in;
    int pixel_x, pixel_y;
    float ccd_size_x, ccd_size_y, focal;
    float camera_center_x, camera_center_y, camera_center_z;
    float focal_center_x, focal_center_y, focal_center_z;
    float up_x, up_y, up_z;

    in.open(config.c_str());
    if (!in.is_open())
    {
        cerr << "FAIL TO OPEN FILE" << endl;
        throw exception();
    }

    // load main camera
    in >> pixel_x >> pixel_y >> ccd_size_x >> ccd_size_y >> focal;
    in >> camera_center_x >> camera_center_y >> camera_center_z;
    in >> focal_center_x >> focal_center_y >> focal_center_z;
    in >> up_x >> up_y >> up_z;
    cout << camera_center_x << " " << camera_center_y << " " << camera_center_z << endl;
    cout << focal_center_x << " " << focal_center_y << " " << focal_center_z << endl;
    cout << up_x << " " << up_y << " " << up_z << endl;
    cout << "window size: " << pixel_x << "x" << pixel_y << endl;

    auto c = new Camera();
    float4 camera_center(camera_center_x / 100.0f, camera_center_y / 100.0f, camera_center_z / 100.0f, 1.0f);
    float4 focal_center(focal_center_x / 100.0f, focal_center_y / 100.0f, focal_center_z / 100.0f, 1.0f);
    float4 up(up_x, up_y, up_z, 0);
    c->set_viewport(pixel_x, pixel_y, ccd_size_x, ccd_size_y, focal);
    c->set_look_at(camera_center, focal_center, up);
    s->camera = c;

    // load light source parameters
    string type;
    int shadow_x, shadow_y;
    float range_x, luminance;
    float center_x, center_y, center_z;

    while (in >> type)
    {
        if (type == "POINT")
        {
            cout << "light: POINT" << endl;
            in >> shadow_x; // point light case: x == y
            in >> luminance;
            in >> center_x >> center_y >> center_z;
            auto point_light = new PointLight(luminance, shadow_x, center_x, center_y, center_z);
            s->light.push_back(point_light);
            continue;
        } else if (type == "SUN")
        {
            cout << "light: SUN" << endl;
            in >> shadow_x >> shadow_y >> range_x;
            in >> luminance;
            in >> center_x >> center_y >> center_z;
            in >> focal_center_x >> focal_center_y >> focal_center_z;
            in >> up_x >> up_y >> up_z;

            auto sun_light = new SunLight();
            float4 center(center_x / 100.0f, center_y / 100.0f, center_z / 100.0f, 1.0f);
            focal_center << focal_center_x / 100.0f, focal_center_y / 100.0f, focal_center_z / 100.0f, 1.0f;
            up << up_x, up_y, up_z, 0;

            sun_light->set_luminance(luminance);
            sun_light->set_viewport(shadow_x, shadow_y, range_x);
            sun_light->set_look_at(center, focal_center, up);
            s->light.push_back(sun_light);
            continue;
        } else if (type == "SHADOW_ON")
        {
            cout << "SHADOW_ON" << endl;
            s->shadow = true;
            continue;
        } else if (type == "SHADOW_OFF")
        {
            cout << "SHADOW_OFF" << endl;
            s->shadow = false;
            continue;
        } else if (type == "BACK")
        {
            cout << "CULL_MODE BACK" << endl;
            s->face_cull_mode = BACK;
            continue;
        } else if (type == "FRONT")
        {
            cout << "CULL_MODE FRONT" << endl;
            s->face_cull_mode = FRONT;
            continue;
        } else if (type == "NONE")
        {
            cout << "CULL_MODE NONE" << endl;
            s->face_cull_mode = NONE;
            continue;
        } else
        {
            cerr << "TYPE ERROR: " + type << endl;
            throw exception();
        }
    }
    in.close();
    return s;
}

void iodata::read_ply(const string &file_name, Model *model)
{
    string ply_file;
    int num_vertex, num_triangle;
    int vertex_attr_size = 0;
    int face_attr_size = 0;
    ifstream in;

    in.open(file_name.c_str());
    if (!in.is_open())
    {
        cerr << "FAIL TO OPEN FILE" << endl;
        throw exception();
    }
    in >> ply_file;
    in.close();

    in.open(ply_file.c_str());
    if (!in.is_open())
    {
        cerr << "FAIL TO OPEN FILE" << endl;
        throw exception();
    }
    string temp;
    in >> temp;
    while (!in.eof())
    {
        //        cout << temp << endl;
        if (temp == "ply")
        {
            in >> temp;
            continue;
        }
        if (temp == "format")
        {
            getline(in, temp, '\n');
            in >> temp;
            continue;
        }
        if (temp == "comment")
        {
            getline(in, temp, '\n');
            in >> temp;
            continue;
        }
        if (temp == "element")
        {
            in >> temp;
            if (temp == "vertex")
            {
                in >> num_vertex;
                while (!in.eof())
                {
                    in >> temp;
                    if (temp == "property")
                    {
                        getline(in, temp, '\n');
                        vertex_attr_size++;
                    } else
                    {
                        break;
                    }
                }
                continue;
            }

            if (temp == "face")
            {
                in >> num_triangle;
                while (!in.eof())
                {
                    in >> temp;
                    if (temp == "property")
                    {
                        getline(in, temp, '\n');
                        face_attr_size++;
                    } else
                    {
                        break;
                    }
                }
                continue;
            }
        }

        if (temp == "end_header")
        {
            break;
        }

        cerr << "FILE FORMAT ERROR" << endl;
        throw exception();
    }

    auto *mesh = new Mesh();
    float x, y, z, u, v;
    int r, g, b, a;
    float4 world, color;
    float2 uv;
    // to calculate vertex normal
    float4 *normal_sum = new float4[num_vertex]; // sum of triangle normal vector contains vertex
    int *triangle_num_index = new int[num_vertex]; // triangle number contains vertex
    mesh->vertices.reserve(num_vertex);
    mesh->triangles.reserve(num_triangle);
    for (int i = 0; i < num_vertex; i++)
    {
        normal_sum[i] << 0, 0, 0, 0;
        triangle_num_index[i] = 0;
    }

    for (int i = 0; i < num_vertex; i++)
    {
        switch (vertex_attr_size)
        {
            case 3:
            {
                in >> x >> y >> z;
                world << x, y, z, 1.0f;
                color << 1.0f, 1.0f, 1.0f, 1.0f;
                Vertex vertex(world, color);
                mesh->add_vertex(vertex);
                break;
            }
            case 5:
            {
                in >> x >> y >> z >> u >> v;
                world << x, y, z, 1.0f;
                color << 1.0f, 1.0f, 1.0f, 1.0f;
                uv << u, 1 - v; // OpenCV read from left-up, to convert the coordinate center to left-down
                Vertex vertex(world, uv);
                mesh->add_vertex(vertex);
                break;
            }
            case 6:
            {
                in >> x >> y >> z >> r >> g >> b;
                world << x, y, z, 1.0f;
                color << (float)r / 255.0f, float(g) / 255.0f, float(b) / 255.0f, 1.0f;
                Vertex vertex(world, color);
                mesh->add_vertex(vertex);
                break;
            }
            case 7:
            {
                in >> x >> y >> z >> r >> g >> b >> a;
                world << x, y, z, 1.0f;
                color << (float)r / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f;
                Vertex vertex(world, color);
                mesh->add_vertex(vertex);
                break;
            }
            default:
                break;
        }
    }

    for (int i = 0; i < num_triangle; i++)
    {
        int num, index_0, index_1, index_2, index_3;
        in >> num;
        if (num == 3)
        {
            in >> index_0 >> index_1 >> index_2;
            Triangle triangle(mesh->vertices, index_0, index_1, index_2);
            mesh->add_triangle(triangle);
            normal_sum[index_0] += triangle.normal;
            normal_sum[index_1] += triangle.normal;
            normal_sum[index_2] += triangle.normal;
            triangle_num_index[index_0]++;
            triangle_num_index[index_1]++;
            triangle_num_index[index_2]++;
        } else if (num == 4)
        {
            in >> index_0 >> index_1 >> index_2 >> index_3;
            Triangle triangle_0(mesh->vertices, index_0, index_1, index_2);
            Triangle triangle_1(mesh->vertices, index_0, index_2, index_3);
            mesh->add_triangle(triangle_0);
            mesh->add_triangle(triangle_1);
            normal_sum[index_0] += triangle_0.normal;
            normal_sum[index_1] += triangle_0.normal;
            normal_sum[index_2] += triangle_0.normal;
            triangle_num_index[index_0]++;
            triangle_num_index[index_1]++;
            triangle_num_index[index_2]++;
            normal_sum[index_0] += triangle_1.normal;
            normal_sum[index_2] += triangle_1.normal;
            normal_sum[index_3] += triangle_1.normal;
            triangle_num_index[index_0]++;
            triangle_num_index[index_2]++;
            triangle_num_index[index_3]++;
        }
    }
    in.close();

    // calculate vertex normal
    for (int i = 0; i < num_vertex; i++)
    {
        mesh->vertices[i].normal = normal_sum[i] / (float)triangle_num_index[i];
    }
    delete[] normal_sum;
    delete[] triangle_num_index;

    cout << "vertex number: " << mesh->num_vertex << endl;
    cout << "triangles number: " << mesh->num_triangle << endl;

    // load material source parameters
    in.open(file_name.c_str());
    if (!in.is_open())
    {
        cerr << "FAIL TO OPEN FILE" << endl;
        throw exception();
    }

    string type;
    float ka, kd, ks, spec_rank;
    in >> ply_file;
    in >> type;
    in >> ka >> kd >> ks >> spec_rank;
    cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
    cout << "specular rank: " << spec_rank << endl;

    Material *m;
    if (type == "FLAT")
    {
        m = new FlatMaterial();
        m->ambient = ka;
        m->diffuse = kd;
        m->specular = ks;
        m->spec_rank = spec_rank;
        mesh->material = m;
    } else if (type == "PHONG")
    {
        m = new PhongMaterial();
        m->ambient = ka;
        m->diffuse = kd;
        m->specular = ks;
        m->spec_rank = spec_rank;
        mesh->material = m;
    } else
    {
        cerr << "MATERIAL TYPE ERROR" << endl;
        throw exception();
    }

    // load base_texture
    string texture_file;
    while (!in.eof())
    {
        in >> type;
        if (type == "TEXTURE")
        {
            in >> texture_file;
            auto *texture = new Texture(texture_file);
            m->set_texture(texture);
            cout << "base_texture: " << texture_file << endl;
        } else
        {
            break;
        }
    }

    in.close();
    model->meshes.push_back(mesh);
}

void iodata::write_ply(Mesh *mesh, const string &file_name)
{
    string outfile = "Data/" + file_name;
    cout << "Writing to " << outfile << endl;
    std::ofstream out;
    out.open(outfile.c_str());

    out << "ply" << endl;
    out << "format ascii 1.0" << endl;
    out << "element vertex " << mesh->num_vertex << endl;
    out << "property float x" << endl;
    out << "property float y" << endl;
    out << "property float z" << endl;
    out << "property uchar red" << endl;
    out << "property uchar green" << endl;
    out << "property uchar blue" << endl;
    out << "property uchar alpha" << endl;
    //    out << "property ushort gray" << endl;
    out << "element face " << mesh->num_triangle << endl;
    out << "property list uchar int vertex_indices" << endl;
    out << "end_header" << endl;

    for (auto vertex: mesh->vertices)
    {
        out << vertex.world.x() << " " << vertex.world.y() << " " << vertex.world.z() << " " << 255 << " " << 255 << " "
            << 255 << " " << 255 << endl;
    }

    for (const auto &triangle: mesh->triangles)
    {
        out << "3 " << triangle.vertex_0 << " " << triangle.vertex_1 << " " << triangle.vertex_2 << endl;
    }

    out.close();
}

void iodata::write_depth_image(const float *z_buffer, Camera *c)
{
    cout << "rendering..." << endl;
    string file = "Data/Output/depth_image.png";

    uint16_t z_lut[65536];
    // update the lut
    for (int i = 0; i < 65536; i++)
    {
        z_lut[i] = (uint16_t)(pow(double(i) / 65535.0, GAMMA) * 65535.0); // 16 bit gray
    }

    cv::Mat image = cv::Mat::zeros(c->y, c->x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < c->y * c->x; i++)
    {
        *p = 65535 - z_lut[(uint16_t)(z_buffer[i] * 65535.0f)];
        p++;
    }
    cv::imwrite(file, image);
}

void iodata::write_depth_image(SunLight *light)
{
    cout << "rendering..." << endl;
    string file = "Data/Output/depth_image.png";

    uint16_t z_lut[65536]; // look up table
    // update the lut
    for (int i = 0; i < 65536; i++)
    {
        z_lut[i] = (uint16_t)(pow(double(i) / 65535.0, Z_GAMMA) * 65535.0); // 16 bit gray
    }

    cv::Mat image = cv::Mat::zeros(light->y, light->x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < light->y * light->x; i++)
    {
        *p = 65535 - z_lut[(uint16_t)(light->shadow_map->map[i] * 65535.0f)];
        p++;
    }
    cv::imwrite(file, image);
}

//void iodata::write_result_image(const FrameBuffer &frame)
//{
//    cout << "output image rendering..." << endl;
//    string file = "data/image.png";
//
//    uint16_t lut[65536]; // look up table
//    // update the lut
//    for (int i = 0; i < 65536; i++)
//    {
//        lut[i] = (uint16_t)(pow(double(i) / 65535.0, GAMMA) * 65535.0); // 16 bit gray
//    }
//
//    cv::Mat image = cv::Mat::zeros(frame.y, frame.x, CV_16U);
//    auto *p = (uint16_t *)image.data;
//    for (int i = 0; i < frame.x * frame.y; i++)
//    {
//        *p = lut[(uint16_t)(frame.buffer[i] * 65535.0f)];
//        p++;
//    }
//    cv::imwrite(file, image);
//}

void iodata::write_result_image(const FrameBuffer &frame)
{
    cout << "output image rendering..." << endl;
    string file = "Data/Output/image.png";

    unsigned char lut[256]; // look up table
    // update the gamma correction lut
    for (int i = 0; i < 256; i++)
    {
        lut[i] = (uint8_t)(pow(double(i) / 255.0, GAMMA) * 255.0); // 8 bit
    }

    cv::Mat image = cv::Mat::zeros(frame.y, frame.x, CV_8UC4);
    auto *p = (uint8_t *)image.data;
    int size = 4 * frame.x * frame.y;
    for (int i = 0; i < size; i += 4)
    {
        //        *p = lut[frame.buffer[i + 2]]; // B
        //        p++;
        //        *p = lut[frame.buffer[i + 1]]; // G
        //        p++;
        //        *p = lut[frame.buffer[i]]; // R
        //        p++;
        //        *p = frame.buffer[i + 3]; // Alpha channel
        //        p++;
        *p = frame.buffer[i + 2]; // B
        p++;
        *p = frame.buffer[i + 1]; // G
        p++;
        *p = frame.buffer[i]; // R
        p++;
        *p = frame.buffer[i + 3]; // Alpha channel
        p++;
    }
    cv::imwrite(file, image);
}

void iodata::write_result_image(const string &file_name, const FrameBuffer &frame)
{
    cout << "writing " << file_name << endl;
    //    unsigned char lut[256]; // look up table
    // update the gamma correction lut
    //    for (int i = 0; i < 256; i++)
    //    {
    //        lut[i] = (uint8_t)(pow(double(i) / 255.0, GAMMA) * 255.0); // 8 bit
    //    }

    cv::Mat image = cv::Mat::zeros(frame.y, frame.x, CV_8UC4);
    auto *p = (uint8_t *)image.data;
    int size = 4 * frame.x * frame.y;
    for (int i = 0; i < size; i += 4)
    {
        //        *p = lut[frame.buffer[i + 2]]; // B
        //        p++;
        //        *p = lut[frame.buffer[i + 1]]; // G
        //        p++;
        //        *p = lut[frame.buffer[i]]; // R
        //        p++;
        //        *p = frame.buffer[i + 3]; // Alpha channel
        //        p++;
        *p = frame.buffer[i + 2]; // B
        p++;
        *p = frame.buffer[i + 1]; // G
        p++;
        *p = frame.buffer[i]; // R
        p++;
        *p = frame.buffer[i + 3]; // Alpha channel
        p++;
    }
    cv::imwrite(file_name, image);
}

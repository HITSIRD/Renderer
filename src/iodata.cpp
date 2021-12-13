//
// Created by 闻永言 on 2021/7/10.
//

#include "iodata.hpp"
#include "Util.hpp"
#include <iostream>
#include <fstream>
#include "FlatMaterial.hpp"
#include "PhongMaterial.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace Render;

void iodata::readDEM(const string &file_name, Model *model)
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

    offset_x = (float)(x - 1) * sample * 0.5f;
    offset_y = (float)(y - 1) * sample * 0.5f;

    auto *mesh = new Mesh();
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            float4 c(float(i) * sample - offset_x, float(j) * sample - offset_y, DEM[i * x + j], 1.0f);
            Vertex v(c);
            mesh->addVertex(v);
        }
    }

    for (int i = 0; i < y - 1; i++)
    {
        for (int j = 0; j < x - 1; j++)
        {
            Triangle triangle_0(mesh->vertices, i * x + j, i * x + x + j, i * x + j + 1);
            Triangle triangle_1(mesh->vertices, i * x + j + 1, i * x + x + j, i * x + x + j + 1);
            mesh->addTriangle(triangle_0);
            mesh->addTriangle(triangle_1);
        }
    }

    cout << "vertex number: " << mesh->numVertices << endl;
    cout << "triangles number: " << mesh->numTriangles << endl;

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
        m->specRank = spec_rank;
        mesh->material = m;
    } else if (type == "PHONG")
    {
        Material *m = new PhongMaterial();
        m->ambient = ka;
        m->diffuse = kd;
        m->specular = ks;
        m->specRank = spec_rank;
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

State *iodata::loadConfig(const string &config)
{
    auto *s = new State();
    cout << config << endl;

    ifstream in;
    int pixelX, pixelY;
    float ccdSizeX, ccdSizeY, focal;
    float cameraCenterX, cameraPositionY, cameraPositionZ;
    float focalX, focalY, focalZ;
    float upX, upY, upZ;

    in.open(config.c_str());
    if (!in.is_open())
    {
        cerr << "FAIL TO OPEN FILE" << endl;
        throw exception();
    }

    // load main camera
    in >> pixelX >> pixelY >> ccdSizeX >> ccdSizeY >> focal;
    in >> cameraCenterX >> cameraPositionY >> cameraPositionZ;
    in >> focalX >> focalY >> focalZ;
    in >> upX >> upY >> upZ;
    //    cout << camera_center_x << " " << camera_center_y << " " << camera_center_z << endl;
    //    cout << focal_center_x << " " << focal_center_y << " " << focal_center_z << endl;
    //    cout << up_x << " " << up_y << " " << up_z << endl;
    cout << "window size: " << pixelX << "x" << pixelY << endl;

    auto c = new Camera();
    float4 cameraPosition(cameraCenterX / SCALE, cameraPositionY / SCALE, cameraPositionZ / SCALE, 1.0f);
    float4 cameraFocal(focalX / SCALE, focalY / SCALE, focalZ / SCALE, 1.0f);
    float4 cameraUp(upX, upY, upZ, 0);

    c->setViewport(pixelX, pixelY, ccdSizeX, ccdSizeY, focal);
    c->setLookAt(cameraPosition, cameraFocal, cameraUp);
    s->camera = c;

    // load light source parameters
    string type;
    size_t numSample;
    int shadowX, shadowY;
    float range, intensity, power;
    float positionX, positionY, positionZ;

    while (in >> type)
    {
        if (type == "POINT")
        {
            cout << "light: POINT" << endl;
            in >> shadowX; // point light case: x == y
            in >> power;
            in >> positionX >> positionY >> positionZ;
            float4 position(positionX / SCALE, positionY / SCALE, positionZ / SCALE, 1.0f);
            power = power / SCALE / SCALE;
            auto point_light = new PointLight(power, shadowX, position);
            s->lightSource.push_back(point_light);
            continue;
        } else if (type == "SUN")
        {
            cout << "light: SUN" << endl;
            in >> shadowX >> shadowY >> range;
            in >> intensity;
            in >> positionX >> positionY >> positionZ;
            in >> focalX >> focalY >> focalZ;
            in >> upX >> upY >> upZ;

            auto sun_light = new SunLight();
            float4 position(positionX / SCALE, positionY / SCALE, positionZ / SCALE, 1.0f);
            cameraFocal << focalX / SCALE, focalY / SCALE, focalZ / SCALE, 1.0f;
            cameraUp << upX, upY, upZ, 0;
            range /= SCALE;
            sun_light->setIntensity(intensity);
            sun_light->setViewport(shadowX, shadowY, range);
            sun_light->setLookAt(position, cameraFocal, cameraUp);
            s->lightSource.push_back(sun_light);
            continue;
        } else if (type == "SHADOW")
        {
            cout << "SHADOW ";
            in >> type;
            if (type == "ON")
            {
                cout << "ON" << endl;
                s->shadow = SHADOW;
            } else if (type == "OFF")
            {
                cout << "OFF" << endl;
                s->shadow = NO_SHADOW;
            } else
            {
                cerr << "SHADOW TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "CULL_MODE")
        {
            cout << "CULL_MODE ";
            in >> type;
            if (type == "BACK")
            {
                cout << "BACK" << endl;
                s->faceCullMode = BACK;
            } else if (type == "FRONT")
            {
                cout << "FRONT" << endl;
                s->faceCullMode = FRONT;
            } else if (type == "NONE")
            {
                cout << "CULL_MODE NONE" << endl;
                s->faceCullMode = NONE;
            } else
            {
                cerr << "CULL MODE TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "TEXTURE_TYPE")
        {
            cout << "TEXTURE_TYPE ";
            in >> type;
            if (type == "NORMAL")
            {
                cout << "NORMAL" << endl;
                s->textureType = NORMAL_TEXTURE;
            } else if (type == "MIPMAP")
            {
                cout << "MIPMAP" << endl;
                s->textureType = MIPMAP;
            } else
            {
                cerr << "TEXTURE TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "TEXTURE_SAMPLER")
        {
            cout << "TEXTURE_SAMPLER ";
            in >> type;
            if (type == "NORMAL")
            {
                cout << "NORMAL" << endl;
                s->sampler = NORMAL;
            } else if (type == "BILINEAR")
            {
                cout << "BILINEAR" << endl;
                s->sampler = BILINEAR;
            } else if (type == "TRILINEAR")
            {
                cout << "TRILINEAR" << endl;
                s->sampler = TRILINEAR;
            } else if (type == "ANISOTROPIC")
            {
                cout << "ANISOTROPIC" << endl;
                s->sampler = ANISOTROPIC;
            } else
            {
                cerr << "TEXTURE SAMPLER TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "SAMPLE_MAX")
        {
            cout << "SAMPLE MAX: ";
            in >> numSample;
            s->maxSample = numSample;
            cout << numSample << endl;
        } else
        {
            cerr << "TYPE ERROR: " + type << endl;
            throw exception();
        }
    }
    in.close();
    return s;
}

void iodata::readPly(const string &file_name, Model *model)
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
    //    int *triangle_num_index = new int[num_vertex]; // triangle number contains vertex
    mesh->vertices.reserve(num_vertex);
    mesh->triangles.reserve(num_triangle);
    for (int i = 0; i < num_vertex; i++)
    {
        normal_sum[i] << 0, 0, 0, 0;
        //        triangle_num_index[i] = 0;
    }

    for (int i = 0; i < num_vertex; i++)
    {
        switch (vertex_attr_size)
        {
            case 3:
            {
                in >> x >> y >> z;
                world << x / SCALE, y / SCALE, z / SCALE, 1.0f;
                color << 1.0f, 1.0f, 1.0f, 1.0f;
                Vertex vertex(world, color);
                mesh->addVertex(vertex);
                break;
            }
            case 5:
            {
                in >> x >> y >> z >> u >> v;
                world << x / SCALE, y / SCALE, z / SCALE, 1.0f;
                color << 1.0f, 1.0f, 1.0f, 1.0f;
                uv << u, 1 - v; // OpenCV read from left-up, to convert the coordinate center to left-down
                Vertex vertex(world, uv);
                mesh->addVertex(vertex);
                break;
            }
            case 6:
            {
                in >> x >> y >> z >> r >> g >> b;
                world << x / SCALE, y / SCALE, z / SCALE, 1.0f;
                color << (float)r * Inv255, float(g) * Inv255, float(b) * Inv255, 1.0f;
                Vertex vertex(world, color);
                mesh->addVertex(vertex);
                break;
            }
            case 7:
            {
                in >> x >> y >> z >> r >> g >> b >> a;
                world << x / SCALE, y / SCALE, z / SCALE, 1.0f;
                color << (float)r * Inv255, float(g) * Inv255, float(b) * Inv255, float(a) * Inv255;
                Vertex vertex(world, color);
                mesh->addVertex(vertex);
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
            mesh->addTriangle(triangle);
            normal_sum[index_0] += triangle.normal;
            normal_sum[index_1] += triangle.normal;
            normal_sum[index_2] += triangle.normal;
        } else if (num == 4)
        {
            in >> index_0 >> index_1 >> index_2 >> index_3;
            Triangle triangle_0(mesh->vertices, index_0, index_1, index_2);
            Triangle triangle_1(mesh->vertices, index_0, index_2, index_3);
            mesh->addTriangle(triangle_0);
            mesh->addTriangle(triangle_1);
            normal_sum[index_0] += triangle_0.normal;
            normal_sum[index_1] += triangle_0.normal;
            normal_sum[index_2] += triangle_0.normal;
            normal_sum[index_0] += triangle_1.normal;
            normal_sum[index_2] += triangle_1.normal;
            normal_sum[index_3] += triangle_1.normal;
        }
    }
    in.close();

    // calculate vertex normal
    for (int i = 0; i < num_vertex; i++)
    {
        //        mesh->vertices[i].normal = normal_sum[i] / (float)triangle_num_index[i];
        mesh->vertices[i].normal = normal_sum[i].normalized();
    }
    delete[] normal_sum;
    //    delete[] triangle_num_index;

    cout << "vertex number: " << mesh->numVertices << endl;
    cout << "triangles number: " << mesh->numTriangles << endl;

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
        m->specRank = spec_rank;
        mesh->material = m;
    } else if (type == "PHONG")
    {
        m = new PhongMaterial();
        m->ambient = ka;
        m->diffuse = kd;
        m->specular = ks;
        m->specRank = spec_rank;
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
            auto *texture = new Texture2D(texture_file);
            m->setTexture(texture);
            cout << "base_texture: " << texture_file << endl;
        } else
        {
            break;
        }
    }

    in.close();
    model->meshes.push_back(mesh);
}

void iodata::writePlyFile(Mesh *mesh, const string &file_name)
{
    string outfile = "Data/" + file_name;
    cout << "Writing to " << outfile << endl;
    std::ofstream out;
    out.open(outfile.c_str());

    out << "ply" << endl;
    out << "format ascii 1.0" << endl;
    out << "element vertex " << mesh->numVertices << endl;
    out << "property float x" << endl;
    out << "property float y" << endl;
    out << "property float z" << endl;
    out << "property uchar red" << endl;
    out << "property uchar green" << endl;
    out << "property uchar blue" << endl;
    out << "property uchar alpha" << endl;
    //    out << "property ushort gray" << endl;
    out << "element face " << mesh->numTriangles << endl;
    out << "property list uchar int vertex_indices" << endl;
    out << "end_header" << endl;

    for (auto vertex: mesh->vertices)
    {
        out << vertex.position.x() << " " << vertex.position.y() << " " << vertex.position.z() << " " << 255 << " "
            << 255 << " " << 255 << " " << 255 << endl;
    }

    for (const auto &triangle: mesh->triangles)
    {
        out << "3 " << triangle.vertexIndex[0] << " " << triangle.vertexIndex[1] << " " << triangle.vertexIndex[2]
            << endl;
    }

    out.close();
}

void iodata::writeDepthImage(const float *depth_buffer, Camera *c)
{
    cout << "rendering..." << endl;
    string file = "Data/Output/depth_image.png";

    uint16_t z_lut[65536];
    // update the lut
    for (int i = 0; i < 65536; i++)
    {
        z_lut[i] = (uint16_t)(pow(double(i) * Inv65535, GAMMA) * 65535.0); // 16 bit gray
    }

    cv::Mat image = cv::Mat::zeros(c->y, c->x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < c->y * c->x; i++)
    {
        *p = 65535 - z_lut[(uint16_t)(depth_buffer[i] * 65535.0f)];
        p++;
    }
    cv::imwrite(file, image);
}

void iodata::writeDepthImage(SunLight *light)
{
    cout << "rendering..." << endl;
    string file = "Data/Output/depth_image.png";

    uint16_t z_lut[65536]; // look up table
    // update the lut
    for (int i = 0; i < 65536; i++)
    {
        z_lut[i] = (uint16_t)(pow(double(i) * Inv65535, Z_GAMMA) * 65535.0); // 16 bit gray
    }

    cv::Mat image = cv::Mat::zeros(light->y, light->x, CV_16U);
    auto *p = (uint16_t *)image.data;
    for (int i = 0; i < light->y * light->x; i++)
    {
        *p = 65535 - z_lut[(uint16_t)(light->shadowMap->map[i] * 65535.0f)];
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
//        lut[i] = (uint16_t)(pow(double(i) * Inv65535, GAMMA) * 65535.0); // 16 bit gray
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

void iodata::writeResultImage(FrameBuffer *frame)
{
    cout << "writing image..." << endl;
    string file = "Data/Output/image.png";

    unsigned char lut[256]; // look up table
    // update the gamma correction lut
    for (int i = 0; i < 256; i++)
    {
        lut[i] = (uint8_t)(pow(double(i) / 255.0, GAMMA) * 255.0); // 8 bit
    }

    cv::Mat image = cv::Mat::zeros(frame->y, frame->x, CV_8UC4);
    auto *p = image.data;
    int size = 4 * frame->x * frame->y;
    for (int i = 0; i < size; i += 4)
    {
        *p = frame->buffer[i + 2]; // B
        p++;
        *p = frame->buffer[i + 1]; // G
        p++;
        *p = frame->buffer[i]; // R
        p++;
        *p = frame->buffer[i + 3]; // Alpha channel
        p++;
    }

    //    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    //    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    cv::imwrite(file, image);
}

void iodata::writeResultImage(const string &file_name, FrameBuffer *frame)
{
    cout << "writing " << file_name << endl;
    //    unsigned char lut[256]; // look up table
    // update the gamma correction lut
    //    for (int i = 0; i < 256; i++)
    //    {
    //        lut[i] = (uint8_t)(pow(double(i) / 255.0, GAMMA) * 255.0); // 8 bit
    //    }

    cv::Mat image = cv::Mat::zeros(frame->y, frame->x, CV_8UC4);
    auto *p = image.data;
    int size = 4 * frame->x * frame->y;
    for (int i = 0; i < size; i += 4)
    {
        *p = frame->buffer[i + 2]; // B
        p++;
        *p = frame->buffer[i + 1]; // G
        p++;
        *p = frame->buffer[i]; // R
        p++;
        *p = frame->buffer[i + 3]; // Alpha channel
        p++;
    }
    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    cv::imwrite(file_name, image);
}

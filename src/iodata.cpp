//
// Created by 闻永言 on 2021/7/10.
//

#include "iodata.hpp"
#include "Util.hpp"
#include <iostream>
#include <fstream>
#include "FlatMaterial.hpp"
#include "PhongMaterial.hpp"
#include "SunLightShadowShader.hpp"
#include "PointLightShadowShader.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace Renderer;

Model *iodata::modelConfigParser(const string &configPath) {
    cout << "model config: " << configPath << endl;

    string modelFile, materialFile, attribute;
    Model *model = new Model();
    ifstream in;

    in.open(configPath.c_str());
    if (!in.is_open()) {
        cerr << "FAIL TO OPEN MODEL CONFIG FILE" << endl;
        throw exception();
    }

    in >> attribute;
    while (!in.eof()) {
        if (attribute == "MODEL") {
            in >> modelFile;
            cout << "MODEL: " << modelFile << endl;
            Mesh *mesh = readPlyFile(modelFile);
            cout << "vertex number: " << mesh->numVertices << endl;
            cout << "triangles number: " << mesh->numTriangles << endl;

            if (in >> attribute) {
                if (attribute == "MATERIAL") {
                    in >> materialFile;
                    mesh->material = materialConfigParser(materialFile);
                    model->meshes.push_back(mesh);
                    in >> attribute;
                    continue;
                }
            }

            mesh->material = new FlatMaterial(); // default material type
            model->meshes.push_back(mesh);
            in >> attribute;
            continue;
        } else if (attribute != "MATERIAL") {
            cerr << "ATTRIBUTE TYPE ERROR" << endl;
            throw exception();
        }
        break;
    }

    in.close();
    return model;
}

Material *iodata::materialConfigParser(const string &configPath) {
    cout << "MATERIAL: " << configPath << endl;

    ifstream in;
    Material *material;

    in.open(configPath.c_str());
    if (!in.is_open()) {
        cerr << "FAIL TO OPEN MATERIAL CONFIG FILE" << endl;
        throw exception();
    }

    string attribute;
    float ka, kd, ks, specRank;
    in >> attribute;
    if (attribute == "FLAT") {
        material = new FlatMaterial();

        in >> ka >> kd >> ks >> specRank;
        cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
        cout << "specular rank: " << specRank << endl;
        material->ambient = ka;
        material->diffuse = kd;
        material->specular = ks;
        material->specRank = specRank;
    } else if (attribute == "PHONG") {
        material = new PhongMaterial();

        in >> ka >> kd >> ks >> specRank;
        cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
        cout << "specular rank: " << specRank << endl;
        material->ambient = ka;
        material->diffuse = kd;
        material->specular = ks;
        material->specRank = specRank;
    } else if (attribute == "PBR") {
        material = new PhongMaterial();

        in >> ka >> kd >> ks >> specRank;
        cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
        cout << "specular rank: " << specRank << endl;
        material->ambient = ka;
        material->diffuse = kd;
        material->specular = ks;
        material->specRank = specRank;
    } else {
        cerr << "MATERIAL TYPE ERROR" << endl;
        throw exception();
    }

    // load texture
    string textureFile;
    while (in >> attribute) {
        if (attribute == "TEXTURE_BASE") {
            in >> textureFile;
            auto *texture = new Texture2D<unsigned char>(textureFile);
            material->setTexture(texture, Renderer::TEXTURE_BASE);
            cout << "TEXTURE BASE: " << textureFile << endl;
        } else if (attribute == "TEXTURE_NORMAL") {
            in >> textureFile;
            auto *texture = new Texture2D<unsigned char>(textureFile);
            material->setTexture(texture, Renderer::TEXTURE_NORMAL);
            cout << "TEXTURE NORMAL: " << textureFile << endl;
        } else if (attribute == "TEXTURE_AO") {
            in >> textureFile;
            auto *texture = new Texture2D<unsigned char>(textureFile);
            material->setTexture(texture, Renderer::TEXTURE_AO);
            cout << "TEXTURE AO: " << textureFile << endl;
        } else if (attribute == "TEXTURE_METALNESS") {
            in >> textureFile;
            auto *texture = new Texture2D<unsigned char>(textureFile);
            material->setTexture(texture, Renderer::TEXTURE_METALNESS);
            cout << "TEXTURE METALNESS: " << textureFile << endl;
        } else if (attribute == "TEXTURE_ROUGHNESS") {
            in >> textureFile;
            auto *texture = new Texture2D<unsigned char>(textureFile);
            material->setTexture(texture, Renderer::TEXTURE_ROUGHNESS);
            cout << "TEXTURE ROUGHNESS: " << textureFile << endl;
        } else if (attribute == "TEXTURE_EMISSION") {
            in >> textureFile;
            auto *texture = new Texture2D<unsigned char>(textureFile);
            material->setTexture(texture, Renderer::TEXTURE_EMISSION);
            cout << "TEXTURE EMISSION: " << textureFile << endl;
        } else {
            cerr << "TYPE ERROR" << endl;
            throw exception();
        }
    }

    return material;
}

void iodata::renderingConfigParser(const string &configPath, State *s) {
    cout << "rendering config: " << configPath << endl;

    ifstream in;
    int pixelX, pixelY;
    float ccdSizeX, ccdSizeY, focal;
    float cameraCenterX, cameraPositionY, cameraPositionZ;
    float focalX, focalY, focalZ;
    float upX, upY, upZ;

    in.open(configPath.c_str());
    if (!in.is_open()) {
        cerr << "FAIL TO OPEN RENDERING CONFIG FILE" << endl;
        throw exception();
    }

    // load main camera
    in >> pixelX >> pixelY >> ccdSizeX >> ccdSizeY >> focal;
    in >> cameraCenterX >> cameraPositionY >> cameraPositionZ;
    in >> focalX >> focalY >> focalZ;
    in >> upX >> upY >> upZ;
    cout << "window size: " << pixelX << "x" << pixelY << endl;

    auto c = new Camera();
    float4 cameraPosition(cameraCenterX, cameraPositionY, cameraPositionZ, 1.0f);
    float4 cameraFocal(focalX, focalY, focalZ, 1.0f);
    float4 cameraUp(upX, upY, upZ, 0);

    c->setViewport(pixelX, pixelY, ccdSizeX, ccdSizeY, focal);
    c->setLookAt(cameraPosition, cameraFocal, cameraUp);
    s->camera = c;

    // load light source parameters
    string type;
    size_t numSample;
    int shadowSize;
    float range, intensity, power;
    float positionX, positionY, positionZ;

    while (in >> type) {
        if (type == "POINT") {
            cout << "light: POINT" << endl;
            in >> shadowSize; // point light case: x == y
            in >> power;
            in >> positionX >> positionY >> positionZ;
            float4 position(positionX, positionY, positionZ, 1.0f);
            power = power;
            auto point_light = new PointLight(power, shadowSize, position);
            point_light->shader = PointLightShadowShader::instance();
            s->lightSource.push_back(point_light);
            continue;
        } else if (type == "SUN") {
            cout << "light: SUN" << endl;
            in >> shadowSize >> range >> intensity;
            in >> positionX >> positionY >> positionZ;
            in >> focalX >> focalY >> focalZ;
            in >> upX >> upY >> upZ;

            auto sun_light = new SunLight();
            float4 position(positionX, positionY, positionZ, 1.0f);
            cameraFocal << focalX, focalY, focalZ, 1.0f;
            cameraUp << upX, upY, upZ, 0;
            sun_light->setIntensity(intensity);
            sun_light->setViewport(shadowSize, range);
            sun_light->setLookAt(position, cameraFocal, cameraUp);
            sun_light->shader = SunLightShadowShader::instance();
            s->lightSource.push_back(sun_light);
            continue;
        } else if (type == "SHADOW") {
            cout << "SHADOW ";
            in >> type;
            if (type == "ON") {
                cout << "ON" << endl;
                s->shadow = SHADOW;
            } else if (type == "OFF") {
                cout << "OFF" << endl;
                s->shadow = NO_SHADOW;
            } else {
                cerr << "SHADOW TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "CULL_MODE") {
            cout << "CULL_MODE ";
            in >> type;
            if (type == "BACK") {
                cout << "BACK" << endl;
                s->faceCullMode = BACK;
            } else if (type == "FRONT") {
                cout << "FRONT" << endl;
                s->faceCullMode = FRONT;
            } else if (type == "NONE") {
                cout << "CULL_MODE NONE" << endl;
                s->faceCullMode = NONE;
            } else {
                cerr << "CULL MODE TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "MIPMAP") {
            cout << "MIPMAP";
            s->mipmap = true;
        } else if (type == "TEXTURE_SAMPLER") {
            cout << "TEXTURE_SAMPLER ";
            in >> type;
            if (type == "NORMAL") {
                cout << "NORMAL" << endl;
                s->sampler = NORMAL;
            } else if (type == "BILINEAR") {
                cout << "BILINEAR" << endl;
                s->sampler = BILINEAR;
            } else if (type == "TRILINEAR") {
                cout << "TRILINEAR" << endl;
                s->sampler = TRILINEAR;
            } else if (type == "ANISOTROPIC") {
                cout << "ANISOTROPIC" << endl;
                s->sampler = ANISOTROPIC;
            } else {
                cerr << "TEXTURE SAMPLER TYPE ERROR: " + type << endl;
                throw exception();
            }
        } else if (type == "SAMPLE_MAX") {
            cout << "SAMPLE MAX: ";
            in >> numSample;
            s->maxSample = numSample;
            cout << numSample << endl;
        } else {
            cerr << "TYPE ERROR: " + type << endl;
            throw exception();
        }
    }
    in.close();
}

Mesh *iodata::readPlyFile(const string &plyFilePath) {
    ifstream in;
    int num_vertex, num_triangle;
    int vertex_attr_size = 0;
    int face_attr_size = 0;

    in.open(plyFilePath.c_str());
    if (!in.is_open()) {
        cerr << "FAIL TO OPEN FILE" << endl;
        throw exception();
    }
    string temp;
    in >> temp;
    while (!in.eof()) {
        if (temp == "ply") {
            in >> temp;
            continue;
        }
        if (temp == "format") {
            getline(in, temp, '\n');
            in >> temp;
            continue;
        }
        if (temp == "comment") {
            getline(in, temp, '\n');
            in >> temp;
            continue;
        }
        if (temp == "element") {
            in >> temp;
            if (temp == "vertex") {
                in >> num_vertex;
                while (!in.eof()) {
                    in >> temp;
                    if (temp == "property") {
                        getline(in, temp, '\n');
                        vertex_attr_size++;
                    } else {
                        break;
                    }
                }
                continue;
            }

            if (temp == "face") {
                in >> num_triangle;
                while (!in.eof()) {
                    in >> temp;
                    if (temp == "property") {
                        getline(in, temp, '\n');
                        face_attr_size++;
                    } else {
                        break;
                    }
                }
                continue;
            }
        }

        if (temp == "end_header") {
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

    mesh->vertices.reserve(num_vertex);
    mesh->triangles.reserve(num_triangle);

    for (int i = 0; i < num_vertex; i++) {
        switch (vertex_attr_size) {
            case 3: {
                in >> x >> y >> z;
                world << x, y, z, 1.0f;
                color << 1.0f, 1.0f, 1.0f, 1.0f;
                Vertex vertex(world, color);
                mesh->addVertex(vertex);
                break;
            }
            case 5: {
                in >> x >> y >> z >> u >> v;
                world << x, y, z, 1.0f;
                color << 1.0f, 1.0f, 1.0f, 1.0f;
                uv << u, 1 - v; // OpenCV read from left-up, to convert the coordinate center to left-down
                Vertex vertex(world, uv);
                mesh->addVertex(vertex);
                break;
            }
            case 6: {
                in >> x >> y >> z >> r >> g >> b;
                world << x, y, z, 1.0f;
                color << (float) r * Inv255, float(g) * Inv255, float(b) * Inv255, 1.0f;
                Vertex vertex(world, color);
                mesh->addVertex(vertex);
                break;
            }
            case 7: {
                in >> x >> y >> z >> r >> g >> b >> a;
                world << x, y, z, 1.0f;
                color << (float) r * Inv255, float(g) * Inv255, float(b) * Inv255, float(a) * Inv255;
                Vertex vertex(world, color);
                mesh->addVertex(vertex);
                break;
            }
            default:
                break;
        }
    }

    for (int i = 0; i < num_triangle; i++) {
        int num, index_0, index_1, index_2, index_3;
        in >> num;
        if (num == 3) {
            in >> index_0 >> index_1 >> index_2;
            Triangle triangle(mesh->vertices, index_0, index_1, index_2);
            mesh->addTriangle(triangle);
        } else if (num == 4) {
            in >> index_0 >> index_1 >> index_2 >> index_3;
            Triangle triangle_0(mesh->vertices, index_0, index_1, index_2);
            Triangle triangle_1(mesh->vertices, index_0, index_2, index_3);
            mesh->addTriangle(triangle_0);
            mesh->addTriangle(triangle_1);
        }
    }
    in.close();

    return mesh;
}

Mesh *iodata::readDEM(const string &DEMFile) {
    string DEM_file;
    int x; // DEM resolution
    int y; // DEM resolution
    float offset_x; // make meshes at center
    float offset_y; // make meshes at center
    float sample; // distance between two sample world

    ifstream in;
    in.open(DEMFile.c_str());
    in >> DEM_file;
    in.close();

    in.open(DEM_file.c_str()); // load DEM file
    in >> x >> y >> sample;
    cout << "DEM_x = " << x << ", DEM_y = " << y << ", sample = " << sample << endl;

    auto DEM = new float[x * y];

    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            {
                in >> DEM[i * x + j];
            }
        }
    }
    in.close();

    offset_x = (float) (x - 1) * sample * 0.5f;
    offset_y = (float) (y - 1) * sample * 0.5f;

    auto *mesh = new Mesh();
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            float4 c(float(i) * sample - offset_x, float(j) * sample - offset_y, DEM[i * x + j], 1.0f);
            Vertex v(c);
            mesh->addVertex(v);
        }
    }

    for (int i = 0; i < y - 1; i++) {
        for (int j = 0; j < x - 1; j++) {
            Triangle triangle_0(mesh->vertices, i * x + j, i * x + x + j, i * x + j + 1);
            Triangle triangle_1(mesh->vertices, i * x + j + 1, i * x + x + j, i * x + x + j + 1);
            mesh->addTriangle(triangle_0);
            mesh->addTriangle(triangle_1);
        }
    }

    cout << "vertex number: " << mesh->numVertices << endl;
    cout << "triangles number: " << mesh->numTriangles << endl;

    in.close();
    delete[] DEM;

    return mesh;
}

void iodata::writePlyFile(Mesh *mesh, const string &fileName) {
    string outfile = "Data/" + fileName;
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

    for (auto vertex: mesh->vertices) {
        out << vertex.position.x() << " " << vertex.position.y() << " " << vertex.position.z() << " " << 255 << " "
                << 255 << " " << 255 << " " << 255 << endl;
    }

    for (const auto &triangle: mesh->triangles) {
        out << "3 " << triangle.vertexIndex[0] << " " << triangle.vertexIndex[1] << " " << triangle.vertexIndex[2]
                << endl;
    }

    out.close();
}

void iodata::writeDepthImage(Image<float> *shadowMap) {
    cout << "rendering depth buffer image..." << endl;
    string file = "Data/Output/depth_image.png";

    uint16_t z_lut[65536]; // look up table
    // update the lut
    for (int i = 0; i < 65536; i++) {
        z_lut[i] = (uint16_t) (powf(float(i) * Inv65535, Z_GAMMA) * 65535.f); // 16 bit gray
    }

    cv::Mat image = cv::Mat::zeros(shadowMap->y, shadowMap->x, CV_16U);
    auto *p = (uint16_t *) image.data;

    float min = 1.f;
    for (int i = 0; i < shadowMap->y * shadowMap->x; i++) {
        if (min > shadowMap->data[i]) {
            min = shadowMap->data[i];
        }
    }
    float offset = min;
    float inv = 1.f / (1.f - min);
    for (int i = 0; i < shadowMap->y * shadowMap->x; i++) {
        *p = 65535 - z_lut[(uint16_t) (inv * (shadowMap->data[i] - offset) * 65535.f)];
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

void iodata::writeResultImage(FrameBuffer *frame) {
    cout << "writing image..." << endl;
    string file = "Data/Output/image.png";

    int lut[65536]; // look up table
    // update the gamma correction lut
    for (int i = 0; i < 65536; i++) {
        lut[i] = (int) (pow(double(i) * Inv65535, GAMMA) * 65535.0f); // 16 bit
    }

    cv::Mat image = cv::Mat::zeros(frame->colorBuffer->y, frame->colorBuffer->x, CV_8UC4);
    auto *p = image.data;
    int size = 4 * frame->colorBuffer->x * frame->colorBuffer->y;
    for (int i = 0; i < size; i += 4) {
        *p = lut[(int) (65535.0f * frame->colorBuffer->data[i + 2])] / 256; // B
        p++;
        *p = lut[(int) (65535.0f * frame->colorBuffer->data[i + 1])] / 256; // G
        p++;
        *p = lut[(int) (65535.0f * frame->colorBuffer->data[i])] / 256; // R
        p++;
        *p = 255; // Alpha channel
        p++;
    }
    //    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    //    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    cv::imwrite(file, image);
}

void iodata::writeResultImage(const string &fileName, FrameBuffer *frame) {
    cout << "writing " << fileName << endl;
    unsigned char lut[256]; // look up table
    // update the gamma correction lut
    for (int i = 0; i < 256; i++) {
        lut[i] = (uint8_t) (powf(float(i) * Inv255, GAMMA) * 255.0); // 8 bit
    }

    cv::Mat image = cv::Mat::zeros(frame->colorBuffer->y, frame->colorBuffer->x, CV_8UC4);
    auto *p = image.data;
    int size = 4 * frame->colorBuffer->x * frame->colorBuffer->y;
    for (int i = 0; i < size; i += 4) {
        *p = lut[(int) (255.f * frame->colorBuffer->data[i + 2])]; // B
        p++;
        *p = lut[(int) (255.f * frame->colorBuffer->data[i + 1])]; // G
        p++;
        *p = lut[(int) (255.f * frame->colorBuffer->data[i])]; // R
        p++;
        *p = 255; // Alpha channel
        p++;
    }
    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    cv::pyrDown(image, image, cv::Size(image.rows / 2, image.cols / 2));
    cv::imwrite(fileName, image);
}

//
// Created by 闻永言 on 2021/12/2.
//

#include "RayTracer.hpp"
#include "Material.hpp"
#include "iodata.hpp"
#include "Shader.hpp"
#include "Util.hpp"
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include "Eigen/Geometry"
#include <sys/time.h>

using namespace std;
using namespace Renderer;

void RayTracer::readConfig(const string &modelConfig, const string &renderingConfig) {
    cout << "read config... " << endl;
    struct timeval t0, t1;
    double start_time, end_time, cost;
    // initialize state firstly
    gettimeofday(&t0, NULL);

    s = new State();
    iodata::renderingConfigParser(renderingConfig, s);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load config cost time: " << cost << " ms" << endl;
    cout << "view angle: " << rad2deg(s->camera->FovH) << endl;
    cout << "clipping plane [near, far]: " << s->camera->n << ", " << s->camera->f << endl;

    gettimeofday(&t0, NULL);

    s->model = iodata::modelConfigParser(modelConfig);

    gettimeofday(&t1, NULL);
    start_time = double(t0.tv_usec) / 1000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) * 1000.0 + double(t1.tv_usec) / 1000.0;
    cost = end_time - start_time;
    cout << "load model cost time: " << cost << " ms" << endl;
    //    iodata::write_ply(&s->model->meshes[0]); // write ply file
}

void RayTracer::initialize() {
    cout << "initialize..." << endl;
    frameBuffer = new FrameBuffer(s->camera->x, s->camera->y);
    s->resetBuffer(); // initialize buffers
    frameBuffer->reset();

    // create BVH
    s->model->createBVH();

    //    s->num_threads = omp_get_max_threads();
    //    s->num_threads = 4;
    //    cout << "max_threads: " << s->num_threads << endl;

    // calculate vertex normal
    for (auto &mesh: s->model->meshes) {
        auto *normal_sum = new float4[mesh->numVertices]; // sum of triangle normal vector contains vertex
        for (int i = 0; i < mesh->numVertices; i++) {
            normal_sum[i] << 0, 0, 0, 0;
        }
        for (const auto &tri: mesh->triangles) {
            float4 OA = mesh->vertices[tri.vertexIndex[1]].position - mesh->vertices[tri.vertexIndex[0]].position;
            float4 OB = mesh->vertices[tri.vertexIndex[2]].position - mesh->vertices[tri.vertexIndex[0]].position;
            float4 normal = OA.cross3(OB).normalized(); // normalize
            normal_sum[tri.vertexIndex[0]] += normal;
            normal_sum[tri.vertexIndex[1]] += normal;
            normal_sum[tri.vertexIndex[2]] += normal;
        }
        for (int i = 0; i < mesh->numVertices; i++) {
            mesh->vertices[i].normal = normal_sum[i].normalized();
        }

        delete[] normal_sum;
    }

    // calculate vertex tangent
    for (auto &mesh: s->model->meshes) {
        if (mesh->material->textureNormal) // process only when normal texture is available
        {
            auto *tangentSum = new float4[mesh->numVertices]; // sum of tangent vector of vertices in each face
            for (int i = 0; i < mesh->numVertices; i++) {
                tangentSum[i] << 0, 0, 0, 0;
            }
            for (const auto &tri: mesh->triangles) {
                float4 tangent =
                        calculateTangent(mesh->vertices[tri.vertexIndex[0]], mesh->vertices[tri.vertexIndex[1]],
                                         mesh->vertices[tri.vertexIndex[2]]);
                tangentSum[tri.vertexIndex[0]] += tangent;
                tangentSum[tri.vertexIndex[1]] += tangent;
                tangentSum[tri.vertexIndex[2]] += tangent;
            }
            for (int i = 0; i < mesh->numVertices; i++) {
                mesh->vertices[i].tangent = tangentSum[i].normalized();
            }

            delete[] tangentSum;
        }
    }

    // set triangle attribute
    for (auto &mesh: s->model->meshes) {
        for (auto &triangle: mesh->triangles) {
            triangle.mesh = mesh;
        }
    }

    // initialize random buffer
    randomFloatBuffer = getRandomFloatBuffer(RANDOM_BUFFER_SIZE);
}

void RayTracer::render(Renderer::RenderMode mode) {
    struct timeval start, end, t0, t1;
    double start_time, end_time;

    gettimeofday(&t0, NULL);
    initialize();
    gettimeofday(&t1, NULL);
    start_time = t0.tv_usec / 1000000.0;
    end_time = double(t1.tv_sec - t0.tv_sec) + double(t1.tv_usec) / 1000000;
    double cost = end_time - start_time;
    cout << "initialize cost time: " << cost << " s" << endl;

    if (mode == Renderer::DO_LOOP) {
        //        render_loop();
    } else if (mode == Renderer::OUTPUT_SINGLE) {
        gettimeofday(&start, NULL); // start
        draw();
        gettimeofday(&end, NULL);
        start_time = double(start.tv_usec) / 1000000.0;
        end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
        cost = end_time - start_time;
        cout << "frame cost time: " << cost << " s" << endl;

        iodata::writeResultImage(frameBuffer);
    }
}

void RayTracer::draw() {
    size_t sample = s->maxSample;
    size_t sqrtSample = (int) round(sqrt((float) (s->maxSample)));
    size_t realSample = sqrtSample * sqrtSample;
    float step = 1.0f / (float) (sqrtSample);
    float halfStep = 0.5f * step;
    float4 color_buffer;
    cout << "real sample: " << realSample << endl;
    srand(time(nullptr));

    for (int i = 0; i < s->camera->y; i++) {
        for (int j = 0; j < s->camera->x; j++) {
            color_buffer << 0, 0, 0, 1.0f;
            //            for (int k = 0; k < realSample; k++)
            //            {
            //                auto _x = (float)j + abs(randomFloatBuffer[random() % RANDOM_BUFFER_SIZE]);
            //                auto _y = (float)i + abs(randomFloatBuffer[random() % RANDOM_BUFFER_SIZE]);
            // jittering sampling
            auto _x = (float) j + halfStep;
            auto _y = (float) i + halfStep;
            for (int m = 0; m < sqrtSample; m++) {
                for (int n = 0; n < sqrtSample; n++) {
                    Ray ray =
                            emitRay(_x + (float) n * step +
                                    halfStep * (2.0f * randomFloatBuffer[random() % RANDOM_BUFFER_SIZE] - 1.0f),
                                    _y + (float) m * step + halfStep *
                                    (2.0f * randomFloatBuffer[random() % RANDOM_BUFFER_SIZE] - 1.0f));
                    //                Ray ray = emitRay(_x, _y);
                    color_buffer += routeTracing(ray, 0);;
                }
            }
            //            }
            writeColor(j, i, color_buffer / (float) realSample);
        }
    }
}

inline Ray RayTracer::emitRay(float x, float y) const {
    float4 direction(x, y, 0, 1.0f);
    direction = s->camera->matrixScreenToView * direction;
    direction = direction / direction.w();
    direction.w() = 0;
    direction = (s->camera->matrixViewToWorld * direction).normalized();
    return {s->camera->position, direction};
}

float4 RayTracer::routeTracing(Ray &ray, int depth) const {
    float4 color(0, 0, 0, 1.0f);
    float4 throughput = float4(1, 1, 1, 0);

    while (depth <= 8) {
        HitRecord record;

        if (s->model->BVH->hit(ray, TMin, TMax, record)) {
            record.position = ray.origin + record.t * ray.direction;
            Uniform uniform;
            uniform.viewPosition = ray.origin;
            uniform.lightSource = &s->lightSource;
            record.material->setUniform(uniform);
            s->shader = record.material->getShader(); // set shader for material
            s->shader->setUniform(uniform); // set uniform

            for (auto &light: s->lightSource) {
                HitRecord shadowRec;
                if (light->type == SUN) {
                    auto sun = (SunLight *) light;
                    Ray shadowRay =
                            Ray(record.position + sun->direct * Epsilon, sun->direct); // to avoid self-occlusion

                    if (s->model->BVH->hit(shadowRay, TMin, TMax, shadowRec)) {
                        record.isInShadow.push_back(true);
                    } else {
                        record.isInShadow.push_back(false);
                    }
                } else if (light->type == POINT) {
                    float4 direct = (light->position - record.position).normalized();
                    float t_max = (light->position - record.position).lpNorm<2>();
                    Ray shadow_ray = Ray(record.position + direct * Epsilon, direct); // to avoid self-occlusion

                    if (s->model->BVH->hit(shadow_ray, TMin, TMax, shadowRec)) {
                        if (shadowRec.t < t_max) {
                            record.isInShadow.push_back(true);
                            continue;
                        }
                    }
                    record.isInShadow.push_back(false);
                }
            }

            //            ray.d = -2.0f * record.flat_normal * record.flat_normal.dot(ray.d) + ray.d; // mirror reflection

            srand(time(nullptr));
            float ksi = randomFloatBuffer[random() % RANDOM_BUFFER_SIZE];
            float phi = randomFloatBuffer[random() % RANDOM_BUFFER_SIZE];
            float tmpKsi = Pi2 * ksi;
            float tmpPhi0 = sqrtf(phi);
            float tmpPhi1 = sqrtf(1.0f - phi);
            float tmpPhi2 = sqrtf(1.0f - phi * phi);

            float4 rand_d(cosf(tmpKsi) * tmpPhi0, sinf(tmpKsi) * tmpPhi0, tmpPhi1, 0); // cosine semi-sphere

            float4 X = record.flatNormal.cross3(RandomFloat4).normalized();
            float4 Y = record.flatNormal.cross3(X);
            float4x4 matrixRotate; // rotate random vector to normal space
            matrixRotate << X, Y, record.flatNormal, ZeroFloat4;
            ray.direction = matrixRotate * rand_d.normalized();
            ray.origin = record.position + ray.direction * Epsilon;

            if (depth == 8) {
                if (ksi < 0.5f) {
                    color += throughput.cwiseProduct(s->shader->rayShader(record)) * 2.0f;
                }
            }

            color += throughput.cwiseProduct(s->shader->rayShader(record));
            throughput = throughput.cwiseProduct(record.color);
        } else {
            return color;
        }

        depth++;
    }

    return color;
}

void RayTracer::writeColor(int x, int y, const float4 &color) const {
    int index = y * s->camera->x + x;
    frameBuffer->writeColor(index, color);
}

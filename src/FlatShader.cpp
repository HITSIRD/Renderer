//
// Created by 闻永言 on 2021/8/28.
//

#include "FlatShader.hpp"
#include "Util.hpp"
#include "Eigen/Geometry"

#define MIN_LUM 0.003921569

using namespace std;
using namespace Renderer;

FlatShader *FlatShader::shader = nullptr;

FlatShader::~FlatShader() = default;

FlatShader *FlatShader::instance() {
    if (shader == nullptr) {
        shader = new FlatShader();
    }
    return shader;
}

void FlatShader::destroy() {
    // uniform = nullptr;
    delete shader;
    shader = nullptr;
}

void FlatShader::vertexShader(VertexP &vertex) {
    vertex.screen = uniform.VP * vertex.position;
    vertex.zRec = 1.0f / vertex.screen.w();
}

float4 FlatShader::fragmentShader(Fragment &frag) {
    float4 albedo = frag.color;
    float4 normal = frag.flatNormal;
    float4 color(0, 0, 0, 1.0f);

    if (uniform.textureBase) {
        if (!uniform.mipmap) {
            albedo = uniform.textureBase->sample(frag.textureCoord, uniform.samplerType);
        } else {
            albedo = uniform.textureBase->sample(frag.textureCoord, frag.ddx, frag.ddy, uniform.samplerType);
        }
    }
    if (uniform.textureNormal) {
        // calculate TBN matrix
        float4 tangent = (frag.tangent - frag.tangent.dot(normal) * normal).normalized(); // orthographic tangent
        float4 binormal = normal.cross3(tangent);
        float4x4 matrixTBN;
        matrixTBN << tangent, binormal, normal, ZeroFloat4;
        if (!uniform.mipmap) {
            normal = matrixTBN * (2.f *
                                  (uniform.textureNormal->sample(frag.textureCoord, uniform.samplerType) -
                                   TextureNormalOffset));
        } else {
            normal = matrixTBN * (2.f *
                                  (uniform.textureNormal->sample(frag.textureCoord, frag.ddx, frag.ddy,
                                                                 uniform.samplerType) -
                                   TextureNormalOffset));
        }
    }
    if (uniform.textureEmission) {
        if (!uniform.mipmap) {
            color = uniform.textureEmission->sample(frag.textureCoord, uniform.samplerType);
        } else {
            color = uniform.textureEmission->sample(frag.textureCoord, frag.ddx, frag.ddy, uniform.samplerType);
        }
    }

    float4 view = uniform.viewPosition - frag.world;
    view.normalize();
    normal.normalize();
    color += uniform.ka * albedo; // ambient

    for (const auto &light: *uniform.lightSource) {
        if (light->type == POINT) {
            float s = shadow(light, frag);
            if (s > 0) {
                auto *point = (PointLight *) light;
                float4 dir_light = point->position - frag.world;
                float distance = dir_light.dot(dir_light);
                float lum = light->intensity / distance;

                if (lum > MIN_LUM) {
                    dir_light = dir_light.normalized();

                    float diff = lum * normal.dot(dir_light);
                    diff = max(diff, 0.0f);

                    // Angle between reflected light and viewing
                    float4 bisector = (view + dir_light).normalized();
                    float spec = lum * powf(max(normal.dot(bisector), 0.0f), uniform.specRank);
                    float4 specular(spec, spec, spec, 1.0f);

                    color += s * light->color.cwiseProduct(uniform.kd * diff * albedo + uniform.ks * specular);
                }
            }
        } else if (light->type == SUN) {
            float s = shadow(light, frag);
            if (s > 0) {
                auto *sun = (SunLight *) light;
                float diff = light->intensity * normal.dot(sun->direct);
                diff = max(diff, 0.0f);
                // Angle between reflected light and viewing
                float4 bisector = (view + sun->direct).normalized();
                float spec = light->intensity * powf(max(normal.dot(bisector), 0.0f), uniform.specRank);
                float4 specular(spec, spec, spec, 1.0f);

                color += s * sun->color.cwiseProduct(uniform.kd * diff * albedo + uniform.ks * specular);
            }
        }
    }

    return color;
}

float4 FlatShader::rayShader(HitRecord &record) {
    float4 albedo = record.color;
    float4 normal = record.flatNormal;
    float4 color(0, 0, 0, 1.0f);
    if (uniform.textureBase) {
        albedo = uniform.textureBase->sample(record.textureCoord, uniform.samplerType);
    }
    if (uniform.textureNormal) {
        // calculate TBN matrix
        float4 tangent = (record.tangent - record.tangent.dot(normal) * normal).normalized(); // orthographic tangent
        float4 binormal = normal.cross3(tangent);
        float4x4 matrixTBN;
        matrixTBN << tangent, binormal, normal, ZeroFloat4;
        normal = matrixTBN * (2.f *
                              (uniform.textureNormal->sample(record.textureCoord, uniform.samplerType) -
                               TextureNormalOffset));
    }
    if (uniform.textureEmission) {
        color = uniform.textureEmission->sample(record.textureCoord, uniform.samplerType);
    }

    float4 view = uniform.viewPosition - record.position;
    view.normalize();
    color += uniform.ka * albedo; // ambient
    //    albedo = float4(uniform.ks, uniform.ks, uniform.ks, 1.0f);
    //    color += uniform.ka * albedo; // ambient

    int light_index = 0;
    for (auto &light: *uniform.lightSource) {
        if (light->type == POINT) {
            if (!record.isInShadow[light_index]) {
                float4 dir_light = light->position - record.position;
                float distance = dir_light.dot(dir_light);
                float lum = light->intensity / distance;

                if (lum > MIN_LUM) {
                    dir_light = dir_light.normalized();

                    float diff = lum * normal.dot(dir_light);
                    diff = max(diff, 0.0f);
                    float4 diffuse(diff, diff, diff, 1.0f);

                    // Angle between reflected light and viewing
                    float4 bisector = (view + dir_light).normalized();
                    float spec = lum * powf(max(normal.dot(bisector), 0.0f), uniform.specRank);
                    float4 specular(spec, spec, spec, 1.0f);

                    color += light->color
                            .cwiseProduct(uniform.kd * diffuse.cwiseProduct(albedo) + uniform.ks * specular);
                }
            }
        } else if (light->type == SUN) {
            if (!record.isInShadow[light_index]) {
                auto sun = (SunLight *) light;
                float diff = light->intensity * normal.dot(sun->direct);
                diff = max(diff, 0.0f);
                float4 diffuse(diff, diff, diff, 1.0f);

                // Angle between reflected light and viewing
                float4 bisector = (view + sun->direct).normalized();
                float spec = light->intensity * powf(max(normal.dot(bisector), 0.0f), uniform.specRank);
                float4 specular(spec, spec, spec, 1.0f);

                color += sun->color.cwiseProduct(uniform.kd * diffuse.cwiseProduct(albedo) + uniform.ks * specular);
            }
        }

        light_index++;
    }

    return color;
}

float FlatShader::shadow(Light *light, const Fragment &frag) {
    if (light->type == POINT) {
        auto *point = (PointLight *) light;
        if (point->shadowMap) {
            float4 direction = frag.world - point->position;
            int faceIndex, maxIndex;
            maxIndex = abs(direction.x()) > abs(direction.y()) ? 0 : 1;
            maxIndex = abs(direction[maxIndex]) < abs(direction.z()) ? 2 : maxIndex;
            switch (maxIndex) {
                case 0:
                    faceIndex = direction.x() <= 0 ? 1 : 3;
                    break;
                case 1:
                    faceIndex = direction.y() <= 0 ? 0 : 2;
                    break;
                case 2:
                    faceIndex = direction.z() <= 0 ? 4 : 5;
                    break;
            }

            float4 shadowMapIndex = point->matrixWorldToScreen[faceIndex] * frag.world;
            shadowMapIndex = shadowMapIndex / shadowMapIndex.w();
            int index_x = (int) shadowMapIndex.x();
            int index_y = (int) shadowMapIndex.y();
            if (index_x < 0 || index_x >= point->shadowSize || index_y < 0 || index_y >= point->shadowSize ||
                shadowMapIndex.z() > 1.f) {
                return 1.0f;
            }
            float bias = shadowMapIndex.z() * 0.0001f;
            return shadowMapIndex.z() - bias - (*point->shadowMap)(faceIndex, index_y * point->shadowSize + index_x) > 0
                       ? 0
                       : 1.f;
        }
    } else if (light->type == SUN) {
        auto *sun = (SunLight *) light;
        if (sun->shadowMap) {
            float4 shadowMapIndex = sun->matrixWorldToScreen * frag.world;
            float bias = shadowMapIndex.z() * 0.005f;
            int index_x = (int) shadowMapIndex.x();
            int index_y = (int) shadowMapIndex.y();
            if (index_x < 0 || index_x >= sun->shadowSize || index_y < 0 || index_y >= sun->shadowSize) {
                return 1.0f;
            }

            // shadow blur by 3x3 PCF
            //        int min_x = index_x - 1 < 0 ? 0 : index_x - 1;
            //        int max_x = index_x + 1 >= sun->x ? sun->x - 1 : index_x + 1;
            //        int min_y = index_y - 1 < 0 ? 0 : index_y - 1;
            //        int max_y = index_y + 1 >= sun->y ? sun->y - 1 : index_y + 1;
            //
            //        float shadow = 0;
            //        for (int i = min_y; i <= max_y; i++)
            //        {
            //            for (int j = min_x; j <= max_x; j++)
            //            {
            //                float deviation = shadow_map_index.z() - bias - (*sun->shadowMap)[index_y * sun->size + index_x];
            //                if (deviation > 0)
            //                {
            //                    shadow += 0.04f;
            //                } else
            //                {
            //                    shadow += 1.0f;
            //                }
            //            }
            //        }
            //        return shadow / float((max_x - min_x + 1) * (max_y - min_y + 1));
            return shadowMapIndex.z() - bias - (*sun->shadowMap)[index_y * sun->shadowSize + index_x] > 0 ? 0 : 1.0f;
        }
    }
    return 1.0f;
}

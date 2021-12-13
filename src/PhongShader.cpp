//
// Created by 闻永言 on 2021/8/28.
//

#include "PhongShader.hpp"
#include "Light.hpp"

#define MIN_LUM 0.003921569

using namespace std;
using namespace Render;

PhongShader *PhongShader::shader = nullptr;

PhongShader::~PhongShader() = default;

PhongShader *PhongShader::instance()
{
    if (shader == nullptr)
    {
        shader = new PhongShader();
    }
    return shader;
}

void PhongShader::vertexShader(VertexP &vertex)
{
    vertex.clip = uniform->camera->VP * vertex.position;
    vertex.z_rec = 1.0f / vertex.clip.w();
}

float4 PhongShader::fragmentShader(Fragment &frag)
{
    float4 albedo = frag.color;
    float4 color(0, 0, 0, 1.0f);
    if (uniform->baseTexture)
    {
        if (uniform->textureType == NORMAL_TEXTURE)
        {
            albedo = uniform->baseTexture->sample(frag.textureCoord, uniform->samplerType);
        } else if (uniform->textureType == MIPMAP)
        {
            albedo = uniform->baseTexture->sample(frag.textureCoord, frag.ddx, frag.ddy, uniform->samplerType);
        }
    }
    float4 view = uniform->camera->position - frag.world;
    frag.normal.normalize();
    view.normalize();
    color += uniform->ka * albedo; // ambient

    for (const auto &light: *uniform->lightSource)
    {
        if (light->type == POINT)
        {
            float s = shadow(light, frag);
            if (s > 0)
            {
                auto *point = (PointLight *)light;
                float4 dir_light = point->position - frag.world;
                float distance = dir_light.dot(dir_light);
                float lum = light->intensity / distance;

                if (lum > MIN_LUM)
                {
                    dir_light = dir_light.normalized();

                    float diff = lum * frag.normal.dot(dir_light);
                    diff = max(diff, 0.0f);
                    float4 diffuse(diff, diff, diff, 1.0f);

                    // Angle between reflected light and viewing
                    float4 bisector = (view + dir_light).normalized();
                    float spec = lum * powf(max(frag.normal.dot(bisector), 0.0f), uniform->specRank);
                    float4 specular(spec, spec, spec, 1.0f);

                    color += s * (uniform->kd * diffuse.cwiseProduct(albedo) + uniform->ks * specular);
                }
            }
        } else if (light->type == SUN)
        {
            float s = shadow(light, frag);
            if (s > 0)
            {
                auto *sun = (SunLight *)light;
                float diff = light->intensity * frag.normal.dot(sun->direct);
                diff = max(diff, 0.0f);
                float4 diffuse(diff, diff, diff, 1.0f);

                // Angle between reflected light and viewing
                float4 bisector = (view + sun->direct).normalized();
                float spec = light->intensity * powf(max(frag.normal.dot(bisector), 0.0f), uniform->specRank);
                float4 specular(spec, spec, spec, 1.0f);

                color += s * (uniform->kd * diffuse.cwiseProduct(albedo) + uniform->ks * specular);
            }
        }
    }

    return color;
}

float4 PhongShader::rayShader(HitRecord &record)
{
    float4 albedo = record.color;
    float4 color(0, 0, 0, 1.0f);
    if (uniform->baseTexture)
    {
        if (uniform->textureType == NORMAL_TEXTURE)
        {
            albedo = uniform->baseTexture->sample(record.textureCoord, uniform->samplerType);
        } else if (uniform->textureType == MIPMAP)
        {
//            albedo = uniform->base_texture->sample(record.texture_uv, record.texture_x, record.texture_y, uniform->sampler);
        }
    }
    float4 view = uniform->camera->position - record.position;
    view.normalize();
    color += uniform->ka * albedo; // ambient
    //    albedo = float4(uniform->ks, uniform->ks, uniform->ks, 1.0f);
    //    color += uniform->ka * albedo; // ambient

    int light_index = 0;
    for (auto &light: *uniform->lightSource)
    {
        if (light->type == POINT)
        {
            if (!record.isInShadow[light_index])
            {
                float4 dir_light = light->position - record.position;
                float distance = dir_light.dot(dir_light);
                float lum = light->intensity / distance;

                if (lum > MIN_LUM)
                {
                    dir_light = dir_light.normalized();

                    float diff = lum * record.normal.dot(dir_light);
                    diff = max(diff, 0.0f);
                    float4 diffuse(diff, diff, diff, 1.0f);

                    // Angle between reflected light and viewing
                    float4 bisector = (view + dir_light).normalized();
                    float spec = lum * powf(max(record.normal.dot(bisector), 0.0f), uniform->specRank);
                    float4 specular(spec, spec, spec, 1.0f);

                    color += (uniform->kd * diffuse.cwiseProduct(albedo) + uniform->ks * specular);
                }
            }
        } else if (light->type == SUN)
        {
            if (!record.isInShadow[light_index])
            {
                auto sun = (SunLight *)light;
                float diff = light->intensity * record.normal.dot(sun->direct);
                diff = max(diff, 0.0f);
                float4 diffuse(diff, diff, diff, 1.0f);

                // Angle between reflected light and viewing
                float4 bisector = (view + sun->direct).normalized();
                float spec = light->intensity * powf(max(record.normal.dot(bisector), 0.0f), uniform->specRank);
                float4 specular(spec, spec, spec, 1.0f);

                color += (uniform->kd * diffuse.cwiseProduct(albedo) + uniform->ks * specular);
            }
        }

        light_index++;
    }

    return color;
}

float PhongShader::shadow(Light *light, const Fragment &frag)
{
    if (!light->shadowMap)
    {
        return 1.0f;
    }
    float4 shadow_map_index;
    float bias;
    if (light->type == POINT)
    {
        //        shadow_map_index = light->P * frag.world;
        //        shadow_map_index = shadow_map_index / shadow_map_index.w();
        //        bias = 0.0000005f;
        //        epsilon = 0.000001f;
    } else if (light->type == SUN)
    {
        auto *sun = (SunLight *)light;
        shadow_map_index = sun->matrixWorldToScreen * frag.world;
        bias = shadow_map_index.z() * 0.005f;
        int index_x = (int)shadow_map_index.x();
        int index_y = (int)shadow_map_index.y();
        if (index_x < 0 || index_x >= sun->x || index_y < 0 || index_y >= sun->y)
        {
            return 1.0f;
        }

        //         shadow blur by 3x3 PCF
        int min_x = index_x - 1 < 0 ? 0 : index_x - 1;
        int max_x = index_x + 1 >= sun->x ? sun->x - 1 : index_x + 1;
        int min_y = index_y - 1 < 0 ? 0 : index_y - 1;
        int max_y = index_y + 1 >= sun->y ? sun->y - 1 : index_y + 1;

        // shadow blur by 5x5 PCF
        //        int min_x = index_x - 2 < 0 ? 0 : index_x - 2;
        //        int max_x = index_x + 2 >= sun->x ? sun->x - 1 : index_x + 2;
        //        int min_y = index_y - 2 < 0 ? 0 : index_y - 2;
        //        int max_y = index_y + 2 >= sun->y ? sun->y - 1 : index_y + 2;

        float shadow = 0;
        for (int i = min_y; i <= max_y; i++)
        {
            for (int j = min_x; j <= max_x; j++)
            {
                float depth = shadow_map_index.z() - bias; // bias
                //            float deviation = abs(depth - light->shadow_map->map[index_y * sun->x + index_x]);
                float deviation = depth - sun->shadowMap->map[i * sun->x + j];
                if (deviation > 0)
                {
                    shadow += 0.01f;
                } else
                {
                    shadow += 1.0f;
                }
            }
        }
        return shadow / float((max_x - min_x + 1) * (max_y - min_y + 1));
        //        return shadow_map_index.z() - bias - sun->shadow_map->map[index_y * sun->x + index_x] > 0 ? 0 : 1.0f;
    }
    return 1.0f;
}

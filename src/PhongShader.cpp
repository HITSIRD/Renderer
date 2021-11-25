//
// Created by 闻永言 on 2021/8/28.
//

#include "PhongShader.hpp"
#include "Light.hpp"

#define MIN_LUM 0.003921569

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

void PhongShader::vertex_shader(VertexP &vertex)
{
    vertex.clip = uniform->camera->VP * vertex.position;
    vertex.z_rec = 1.0f / vertex.clip.w();
}

void PhongShader::fragment_shader(Fragment &frag)
{
    float4 albedo = frag.color;
    float4 color(0, 0, 0, 0);
    if (uniform->base_texture)
    {
        if (uniform->texture_type == NORMAL_TEXTURE)
        {
            albedo = uniform->base_texture->sample(frag.texture_uv, uniform->sampler);
        } else if (uniform->texture_type == MIPMAP)
        {
            albedo = uniform->base_texture->sample(frag.texture_uv, frag.texture_x, frag.texture_y, uniform->sampler);
        }
    }
    float4 view = uniform->camera->camera_center - frag.world;
    frag.normal.normalize();
    view.normalize();
    color += uniform->ka * albedo; // ambient

    for (const auto &light: *uniform->light_source)
    {
        if (light->type == POINT)
        {
            float s = shadow(light, frag);
            if (s > 0)
            {
                auto *point = (PointLight *)light;
                float4 dir_light = point->center - frag.world;
                float distance = dir_light.dot(dir_light);
                float lum = light->luminance / distance;

                if(lum > MIN_LUM)
                {
                    dir_light = dir_light.normalized();

                    float diff = lum * frag.normal.dot(dir_light);
                    diff = max(diff, 0.0f);
                    float4 diffuse(diff, diff, diff, 1.0f);

                    // Angle between reflected light and viewing
                    float4 bisector = (view + dir_light).normalized();
                    float spec = lum * powf(max(frag.normal.dot(bisector), 0.0f), uniform->spec_rank);
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
                float diff = light->luminance * frag.normal.dot(sun->direct);
                diff = max(diff, 0.0f);
                float4 diffuse(diff, diff, diff, 1.0f);

                // Angle between reflected light and viewing
                float4 bisector = (view + sun->direct).normalized();
                float spec = light->luminance * powf(max(frag.normal.dot(bisector), 0.0f), uniform->spec_rank);
                float4 specular(spec, spec, spec, 1.0f);

                color += s * (uniform->kd * diffuse.cwiseProduct(albedo) + uniform->ks * specular);
            }
        }
    }

    frag.color = color;
}

float PhongShader::shadow(Light *light, const Fragment &frag)
{
    if (!light->shadow_map)
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
        shadow_map_index = sun->MO * frag.world;
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
                float deviation = depth - sun->shadow_map->map[i * sun->x + j];
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

//
// Created by 闻永言 on 2021/8/15.
//

#include "FragmentShader.hpp"

//
// Created by 闻永言 on 2021/8/7.
//

#include <iostream>

using namespace Eigen;
using namespace std;

void FragmentShader::shading(Fragment &fragment, int shading_type)
{
    switch (shading_type)
    {
        case FLAT_SHADING:
            flat_shading(fragment);
            break;
        case PHONG_SHADING:
            phong_shading(fragment);
            break;
        default:;
    }
}

void FragmentShader::flat_shading(Fragment &fragment)
{
    float lum = fragment.normal.dot(-light);
    //    float lum = abs(normal.dot(light) / (normal.norm() * light.norm()));
    float diffuse = max(lum, 0.0f);

    // Angle between reflected light and viewing
    Vector4f bisector = (view - light).normalized();
    //    float specular = pow(max(normal.dot(bisector) / (normal.norm() * bisector.norm()), 0.0), spec_rank);
    //    float specular = powf(max(fragment.normal.dot(bisector), 0.0f), spec_rank);
    float specular = max(fragment.normal.dot(bisector), 0.0f);
    fragment.color = ka + kd * diffuse + ks * specular;
}

void FragmentShader::phong_shading(Fragment &fragment)
{
    // normal vector interpolation
    float lum = fragment.normal.dot(-light);
    float diffuse = max(lum, 0.0f);

    // Angle between reflected light and viewing
    Vector4f bisector = (view - light).normalized();
//    float specular = powf(max(fragment.normal.dot(bisector), 0.0f), spec_rank);
    float specular = max(fragment.normal.dot(bisector), 0.0f);
    fragment.color = ka + kd * diffuse + ks * specular;
}

//
// Created by 闻永言 on 2021/8/15.
//

#include "VertexShader.hpp"

using namespace Eigen;

void VertexShader::shading(Vertex &vertex, int shading_type)
{
    switch (shading_type)
    {
        case FLAT_SHADING:
            flat_shading(vertex);
            break;
        case PHONG_SHADING:
            phong_shading(vertex);
            break;
        default:;
    }
}

void VertexShader::flat_shading(Vertex &vertex)
{
    float lum = vertex.normal.dot(-light);
    //    float lum = abs(normal.dot(light) / (normal.norm() * light.norm()));
    float diffuse = fmax(lum, 0.0f);

    // Angle between reflected light and viewing
    Vector4f bisector = ((-light) + view).normalized();
    //    float specular = pow(max(normal.dot(bisector) / (normal.norm() * bisector.norm()), 0.0), spec_rank);
    float specular = pow(fmax(vertex.normal.dot(bisector), 0.0f), spec_rank);
    vertex.color = ka + kd * diffuse + ks * specular;
}

void VertexShader::phong_shading(Vertex &vertex)
{
    // normal vector interpolation
    float lum = vertex.normal.dot(-light);
    float diffuse = fmax(lum, 0.0f);
    //    float diffuse = max(abs(lum), 0.0);

    // Angle between reflected light and viewing
    Vector4f bisector = ((-light) + view).normalized();
    float specular = pow(fmax(vertex.normal.dot(bisector), 0.0f), spec_rank);
    //    float specular = pow(max(abs(normal.dot(bisector) / (normal.norm() * bisector.norm())), 0.0), spec_rank);
    vertex.color = ka + kd * diffuse + ks * specular;
}

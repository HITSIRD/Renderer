//
// Created by 闻永言 on 2021/8/7.
//

#include <iostream>
#include <fstream>
#include "Shader.hpp"

using namespace Eigen;
using namespace std;

void Shader::read_config(std::string &config_file)
{
    ifstream in;
    in.open(config_file.c_str());

    double norm_x, norm_y, norm_z;
    in >> norm_x >> norm_y >> norm_z;
    cout << "light normal: " << norm_x << " " << norm_y << " " << norm_z << endl;
    light << norm_x, norm_y, norm_z;

    in >> ka >> kd >> spec_rank;
    ks = 1 - ka - kd;
    cout << "ka: " << ka << " kd: " << kd << " ks: " << ks << endl;
    cout << "specular rank: " << spec_rank << endl;

    in.close();
    view << 0, 0, -1.0;
    calibrate();
};

void Shader::calibrate()
{
    //    light = camera->R * light;
    //    light << 0, 0, 1;
    view = camera->R_inv * view;
};

void Shader::flat_shading(Vector3d normal, double *point)
{
    double lum = normal.dot(-light) / (normal.norm() * light.norm());
    //    double lum = abs(normal.dot(light) / (normal.norm() * light.norm()));
    double diffuse = max(lum, 0.0);

    // Angle between reflected light and viewing
    Vector3d bisector = (-light) + view;
    //    double specular = pow(max(normal.dot(bisector) / (normal.norm() * bisector.norm()), 0.0), spec_rank);
    double specular = pow(max(abs(normal.dot(bisector) / (normal.norm() * bisector.norm())), 0.0), spec_rank);
    *point = ka + kd * diffuse + ks * specular;
};

void Shader::phong_shading(
        Vector3d &normal_0, Vector3d &normal_1, Vector3d &normal_2, double u, double v, double w, double *point)
{
    // normal vector interpolation
    Vector3d normal = w * normal_0 + v * normal_1 + u * normal_2;
    double lum = normal.dot(-light) / (normal.norm() * light.norm());
    double diffuse = max(lum, 0.0);
    //    double diffuse = max(abs(lum), 0.0);

    // Angle between reflected light and viewing
    Vector3d bisector = (-light) + view;
    double specular = pow(max(normal.dot(bisector) / (normal.norm() * bisector.norm()), 0.0), spec_rank);
    //    double specular = pow(max(abs(normal.dot(bisector) / (normal.norm() * bisector.norm())), 0.0), spec_rank);
    *point = ka + kd * diffuse + ks * specular;
};

void Shader::phong_shading(Vector3d &normal, double *point)
{
    // normal vector interpolation
    double lum = normal.dot(-light) / (normal.norm() * light.norm());
    double diffuse = max(lum, 0.0);

    // Angle between reflected light and viewing
    Vector3d bisector = (-light) + view;
    double specular = pow(max(normal.dot(bisector) / (normal.norm() * bisector.norm()), 0.0), spec_rank);
    *point = ka + kd * diffuse + ks * specular;
};

//
// Created by 闻永言 on 2021/7/10.
//

#include "iodata.hpp"
#include <iostream>
#include <fstream>

using namespace std;

void iodata::dem2mesh(Mesh &mesh)
{
    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            Vec3 c(float(i) * sample - offset_x, float(j) * sample - offset_y, DEM[i * x + j], 1);
            Vertex v(c);
            mesh.add_vertex(v);
        }
    }

    for (int i = 0; i < y - 1; i++)
    {
        for (int j = 0; j < x - 1; j++)
        {
            Triangle triangle_0(mesh.vertices, i * x + j, i * x + x + j, i * x + j + 1);
            Triangle triangle_1(mesh.vertices, i * x + j + 1, i * x + x + j, i * x + x + j + 1);
            mesh.add_triangle(triangle_0);
            mesh.add_triangle(triangle_1);
        }
    }
}

void iodata::read_DEM(string &file_name)
{
    ifstream in;

    in.open(file_name.c_str());
    in >> x >> y >> sample;
    cout << "DEM_x = " << x << ", DEM_y = " << y << ", sample = " << sample << endl;

    DEM = new float[x * y];

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
}

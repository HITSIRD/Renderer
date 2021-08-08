//
// Created by 闻永言 on 2021/7/10.
//

#include <vector>
#include "iodata.hpp"

using namespace std;

vector<Eigen::Vector3d> iodata::data::dem2mesh()
{
    vector<Eigen::Vector3d> mesh;
    for (int i = 0; i < x - 1; i++)
    {
        for (int j = 0; j < y - 1; j++)
        {
            Eigen::Vector3d p_0;
            Eigen::Vector3d p_1;
            Eigen::Vector3d p_2;
            Eigen::Vector3d p_3;
            Eigen::Vector3d p_4;
            Eigen::Vector3d p_5;
            p_0 << double(i * sample) - offset_x, double(j * sample) - offset_y, DEM[i * y + j];
            p_1 << double(i * sample) - offset_x, double((j + 1) * sample) - offset_y, DEM[i * y + j + 1];
            p_2 << double((i + 1) * sample) - offset_x, double((j + 1) * sample) - offset_y, DEM[(i + 1) * y + j + 1];
            p_3 << double(i * sample) - offset_x, double(j * sample) - offset_y, DEM[i * y + j];
            p_4 << double((i + 1) * sample) - offset_x, double((j + 1) * sample) - offset_y, DEM[(i + 1) * y + j + 1];
            p_5 << double((i + 1) * sample) - offset_x, double(j * sample) - offset_y, DEM[(i + 1) * y + j];
            mesh.push_back(p_0);
            mesh.push_back(p_1);
            mesh.push_back(p_2);
            mesh.push_back(p_3);
            mesh.push_back(p_4);
            mesh.push_back(p_5);
        }
    }
    return mesh;
}

void iodata::data::read_DEM(string &file_name)
{
    ifstream in;

    in.open(file_name.c_str());
    in >> x >> y >> sample;
    cout << "pixel_x = " << x << ", pixel_y = " << y << ", sample = " << sample << endl;

    DEM = new double[x * y];

    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            {
                in >> DEM[i * y + j];
            }
        }
    }
    in.close();

    offset_x = (x - 1) * sample / 2.0;
    offset_y = (y - 1) * sample / 2.0;
}

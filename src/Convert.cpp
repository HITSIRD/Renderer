//
// Created by 闻永言 on 2021/7/10.
//

#include "../include/Convert.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

void Convert::read_file(string &file_name)
{
    ifstream in;
    cout << file_name << endl;
    int pixel_x, pixel_y;
    float ccd_size_x, ccd_size_y, focal;
    float camera_center_x, camera_center_y, camera_center_z;
    float focal_center_x, focal_center_y, focal_center_z;
    float up_x, up_y, up_z;

    in.open(file_name.c_str());
    in >> pixel_x >> pixel_y >> ccd_size_x >> ccd_size_y >> focal;
    in >> camera_center_x >> camera_center_y >> camera_center_z;
    in >> focal_center_x >> focal_center_y >> focal_center_z;
    in >> up_x >> up_y >> up_z;
    cout << camera_center_x << " " << camera_center_y << " " << camera_center_z << endl;
    cout << focal_center_x << " " << focal_center_y << " " << focal_center_z << endl;
    cout << up_x << " " << up_y << " " << up_z << endl;
    cout << "window size: " << pixel_x << "x" << pixel_y << endl;
    float4 camera_center(camera_center_x / 100.0f, camera_center_y / 100.0f, camera_center_z / 100.0f, 1.0f);
    float4 focal_center(focal_center_x / 100.0f, focal_center_y / 100.0f, focal_center_z / 100.0f, 1.0f);
    float4 up(up_x, up_y, up_z, 0);
    c->set_viewport(pixel_x, pixel_y, ccd_size_x, ccd_size_y, focal);
    c->set_look_at(camera_center, focal_center, up);
    in.close();
}

void Convert::write_result() const
{
    ofstream out;
    out.open("data/cali.txt");
    out << setprecision(PRECISION) << c->M_view << endl;
    cout << setprecision(PRECISION) << c->M_view << endl;
    out.close();
}

Camera *Convert::calibrate(string &file_name)
{
    read_file(file_name);
    c->update();
    write_result();

    return c;
}

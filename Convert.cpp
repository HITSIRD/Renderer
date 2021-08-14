//
// Created by 闻永言 on 2021/7/10.
//

#include "Convert.hpp"

using namespace std;

void Convert::read_file(string &file_name)
{
    ifstream in;
    cout << file_name << endl;
    float ccd_size_x, ccd_size_y, f;
    int pixel_x, pixel_y;
    float x, y, z, h, p, b;

    in.open(file_name.c_str());
    in >> pixel_x >> pixel_y >> ccd_size_x >> ccd_size_y >> f;
    in >> x;
    in >> y;
    in >> z;
    in >> h;
    in >> p;
    in >> b;
    c = new Camera();
    c->read_param(pixel_x, pixel_y, ccd_size_x, ccd_size_y, f, x, y, z, h, p, b);
    in.close();
}

void Convert::render()
{
    ofstream out;
    out.open("data/cali.txt");

    cout << c->x << " " << c->y << " " << c->z << " " << c->h << " " << c->p << " " << c->b << endl;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            out << setprecision(precision) << c->K(i, j) << " ";
            cout << setprecision(precision) << c->K(i, j) << " ";
        }
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            out << setprecision(precision) << c->R(i, j) << " ";
            cout << setprecision(precision) << c->R(i, j) << " ";
        }
    }
    for (int i = 0; i < 3; i++)
    {
        out << setprecision(precision) << c->t(i) << " ";
        cout << setprecision(precision) << c->t(i) << " ";
    }
    for (int i = 0; i < 4; i++)
    {
        out << setprecision(precision) << c->d(i) << " ";
        cout << setprecision(precision) << c->d(i) << " ";
    }
    cout << endl;

    out.close();
}

Camera *Convert::calibrate(string &file_name)
{
    read_file(file_name);
    c->calculate();
    render();

    return c;
}
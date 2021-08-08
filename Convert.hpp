//
// Created by 闻永言 on 2021/7/10.
//

#ifndef DEPTH_SEARCH_CONVERT_HPP
#define DEPTH_SEARCH_CONVERT_HPP

#endif //DEPTH_SEARCH_CONVERT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include "camera.hpp"

#define precision 9

using cv::Mat;

class Convert
{
public:
    Camera *c;

    /**
     * Read file and save c4d parameters to every cameras.
     * @param file_name file name of c4d parameters, format of each line is cF x y z h p b
     */
    void read_file(std::string &file_name);

    void render();

    Camera *calibrate(std::string &file_name);
};
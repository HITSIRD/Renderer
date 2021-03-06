//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_CONVERT_HPP
#define RENDERER_CONVERT_HPP

#include "Camera.hpp"

#define PRECISION 7

class Convert
{
public:
    Camera *c;

    /**
     * Read file and save c4d parameters to every cameras.
     * @param file_name file name of c4d parameters, format of each line is cF x y z h p b
     */
    void read_file(std::string &file_name);

    /**
     *
     */
    void write_result();

    /**
     *
     * @param file_name
     * @return
     */
    Camera *calibrate(std::string &file_name);
};

#endif

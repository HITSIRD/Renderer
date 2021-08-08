//
// Created by 闻永言 on 2021/7/10.
//

#ifndef DEPTH_SEARCH_IODATA_HPP
#define DEPTH_SEARCH_IODATA_HPP

#include "Eigen/Dense"
#include <iostream>
#include <fstream>

namespace iodata
{
    class data
    {
    public:
        /*
         * Original DEM data.
         */
        double *DEM;

        int x;
        int y;
        double sample; // distance between two sample point
        double offset_x; // make model at center
        double offset_y; // make model at center

        /**
         * Convert DEM data to mesh format.
         * @return Mesh matrix like [[x_1, y_1, z_1], [x_2, y_2, z_2], ..., [x_n, y_n, z_n]]
         */
        std::vector<Eigen::Vector3d> dem2mesh();

        /**
         * Read DEM data, the vertex number must be over or equal 2x2.
         * @param file_name DEM file name
         */
        void read_DEM(std::string &file_name);
    };

#endif //DEPTH_SEARCH_IODATA_HPP
}
//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_IODATA_HPP
#define RENDERER_IODATA_HPP

#include <iostream>
#include <fstream>
#include "Eigen/Dense"
#include "Mesh.hpp"

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
         * @return mesh
         */
        Mesh dem2mesh();

        /**
         * Read DEM data, the vertex number must be over or equal 2x2.
         * @param file_name DEM file name
         */
        void read_DEM(std::string &file_name);
    };

}

#endif
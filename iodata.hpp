//
// Created by 闻永言 on 2021/7/10.
//

#ifndef RENDERER_IODATA_HPP
#define RENDERER_IODATA_HPP

#include <iostream>
#include <fstream>
#include "Eigen/Core"
#include "Mesh.hpp"

namespace iodata
{
    class data
    {
    public:
        /*
         * Original DEM data.
         */
        float *DEM;

        uint16_t x; // DEM resolution
        uint16_t y; // DEM resolution
        float sample; // distance between two sample point

        /**
         * Convert DEM data to mesh format.
         * @return mesh
         */
        Mesh *dem2mesh();

        /**
         * Read DEM data, the vertex number must be over or equal 2x2.
         * @param file_name DEM file name
         */
        void read_DEM(std::string &file_name);

    private:
        float offset_x; // make model at center
        float offset_y; // make model at center
    };
}

#endif
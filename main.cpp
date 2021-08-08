#include <iostream>
#include <sys/time.h>
#include "Rasterization.hpp"
#include "Shader.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc > 3)
    {
        string dem_file = argv[1]; // dem file name
        string camera_file = argv[2]; // camera parameters file name
        string config_file = argv[3]; // configuration file name
        Rasterization *raster = new Rasterization();
        raster->read_data(dem_file, camera_file);
        raster->initialize();

        Shader *shader = new Shader(raster->camera);
        shader->read_config(config_file);
        cout << "vertex number :" << raster->mesh.num_vertex << endl;
        cout << "triangles number :" << raster->mesh.num_triangles << endl;
        cout << "image size: " << raster->camera->pixel_x << "x" << raster->camera->pixel_y << endl;

        //        clock_t start = clock();
        struct timeval start, end;
        gettimeofday(&start, NULL);
        raster->rasterize(shader);
        //        clock_t end = clock();
        gettimeofday(&end, NULL);
        double start_time = double(start.tv_usec) / 1000000.0;
        double end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
        cout << "Cost time: " << end_time - start_time << endl;
        raster->write_result_image();
        return 0;
    }
}

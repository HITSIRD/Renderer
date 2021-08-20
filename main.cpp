#include <iostream>
#include <sys/time.h>
#include "Renderer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc > 4)
    {
        string dem_file = argv[1]; // dem file name
        string camera_file = argv[2]; // camera parameters file name
        string config_file = argv[3]; // configuration file name
        int shader_type = atoi(argv[4]); // shading type

        Renderer *render = new Renderer();
        render->read_data(dem_file, camera_file);
        render->read_config(config_file, shader_type);
        cout << "vertex number: " << render->num_vertex() << endl;
        cout << "triangles number: " << render->num_triangle() << endl;
        cout << "image size: " << render->x << "x" << render->y << endl;

        struct timeval start, end;
        gettimeofday(&start, NULL);
        render->render();
        gettimeofday(&end, NULL);
        double start_time = double(start.tv_usec) / 1000000.0;
        double end_time = double(end.tv_sec - start.tv_sec) + double(end.tv_usec) / 1000000.0;
        cout << "cost time: " << end_time - start_time << endl;
        cout << "fps: " << 1/(end_time - start_time) << endl;
        render->write_result_image();
        delete render;
        return 0;
    }
}

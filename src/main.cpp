#include <iostream>
#include "Rasterizer.hpp"
#include "RayTracer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc > 4)
    {
        string modelFile = argv[1]; // model file name
        string configFile = argv[2]; // config file name
        auto renderMode = (Render::RenderMode)atoi(argv[3]); // config file name
        string render = argv[4];

        if(render == "RT")
        {
            auto rayTracer = RayTracer();
            rayTracer.read_data(modelFile, configFile);
            rayTracer.render(renderMode);
        }else
        {
            auto rasterizer = Rasterizer();
            rasterizer.readData(modelFile, configFile);
            rasterizer.render(renderMode);
        }
    }
    return 0;
}

#include <iostream>
#include "Rasterizer.hpp"
#include "RayTracer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc > 3)
    {
        string modelFile = argv[1]; // model file name
        string configFile = argv[2]; // config file name
        auto renderMode = (Renderer::RenderMode)atoi(argv[3]); // config file name
        string render = "R";
        if (argc > 4)
        {
            render = argv[4];
        }

        if (render == "RT")
        {
            auto rayTracer = RayTracer();
            rayTracer.readConfig(modelFile, configFile);
            rayTracer.render(renderMode);
        } else
        {
            auto rasterizer = Rasterizer();
            rasterizer.loadConfig(modelFile, configFile);
            rasterizer.render(renderMode);
        }
    }
    return 0;
}

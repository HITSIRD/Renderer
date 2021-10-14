#include <iostream>
#include "Renderer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc > 3)
    {
        string model_file = argv[1]; // model file name
        string config_file = argv[2]; // config file name
        int mode = atoi(argv[3]); // config file name

        auto *render = new Renderer();
        render->read_data(model_file, config_file);
        render->render(mode);
        delete render;
        return 0;
    }
}


#include "dmx_controller.hpp"

extern "C" void app_main()
{
    static DmxController controller;
    controller.init();
    controller.taskLoop();
}
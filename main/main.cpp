#include "Tasks/foot_switch.hpp"
#include "Tasks/max3485_sender.hpp"
#include "Tasks/nv_storage.hpp"
#include "Tasks/seven_segment_display.hpp"
#include "Tasks/web_server.hpp"
#include "dmx_controller.hpp"

extern "C" void app_main()
{
    Configuration configuration;
    DmxPresets dmxPresets;

    auto *display = new SevenSegmentDisplay();
    auto *footSwitch = new FootSwitch(configuration);
    auto *max3485Sender = new Max3485Sender();
    auto *webServer = new WebServer();
    auto *nvStorage = new NvStorage(configuration, dmxPresets);

    DmxController dmxController(configuration, dmxPresets, display, footSwitch, max3485Sender, webServer, nvStorage);
    dmxController.init();
    dmxController.taskLoop();
}
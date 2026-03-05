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


    Assert assertObj;
    auto *display = new SevenSegmentDisplay(&assertObj);
    auto *footSwitch = new FootSwitch(&assertObj, configuration);
    auto *max3485Sender = new Max3485Sender(&assertObj);
    auto *webServer = new WebServer();
    auto *nvStorage = new NvStorage(&assertObj, configuration, dmxPresets);

    DmxController dmxController(
        &assertObj, configuration, dmxPresets, display, footSwitch, max3485Sender, webServer, nvStorage);
    dmxController.init();
    dmxController.taskLoop();
}
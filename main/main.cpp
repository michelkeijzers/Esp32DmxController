#include "Tasks/dmx_preset_changer.hpp"
#include "Tasks/foot_switch.hpp"
#include "Tasks/max3485_sender.hpp"
#include "Tasks/nv_storage.hpp"
#include "Tasks/seven_segment_display.hpp"
#include "Tasks/web_server.hpp"
#include "dmx_controller.hpp"

extern "C" void app_main()
{

    auto *presetChanger = new DmxPresetChanger();
    auto *display = new SevenSegmentDisplay();
    auto *footSwitch = new FootSwitch();
    auto *max3485Sender = new Max3485Sender();
    auto *webServer = new WebServer();
    auto *nvStorage = new NvStorage();

    DmxController controller(presetChanger, display, footSwitch, max3485Sender, webServer, nvStorage);
    controller.init();
    controller.taskLoop();
}
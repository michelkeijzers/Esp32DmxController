#include "../main/Base/assert.hpp"
#include "../main/Data/configuration.hpp"
#include "../main/Data/dmx_presets.hpp"
#include "../main/Tasks/dmx_controller.hpp"
#include "../main/Tasks/foot_switch.hpp"
#include "../main/Tasks/max3485_sender.hpp"
#include "../main/Tasks/nv_storage.hpp"
#include "../main/Tasks/seven_segment_display.hpp"
#include "../main/Tasks/web_server.hpp"
#include "Mocks/mock_assert.hpp"
#include <gtest/gtest.h>

// System test: Initialization sequence similar to app_main
TEST(SystemTest, Initialization_CallsDmxControllerInit)
{
    MockAssert mockAssert;
    Configuration configuration(&mockAssert);
    DmxPresets dmxPresets(&mockAssert);
    auto *display = new SevenSegmentDisplay(&mockAssert);
    auto *footSwitch = new FootSwitch(&mockAssert, configuration);
    auto *max3485Sender = new Max3485Sender(&mockAssert);
    auto *webServer = new WebServer(&mockAssert);
    auto *nvStorage = new NvStorage(&mockAssert, configuration, dmxPresets);

    DmxController dmxController(
        &mockAssert, configuration, dmxPresets, display, footSwitch, max3485Sender, webServer, nvStorage);
    // Only call init, not taskLoop (which is infinite)
    dmxController.init();

    // Clean up
    delete display;
    delete footSwitch;
    delete max3485Sender;
    delete webServer;
    delete nvStorage;
}

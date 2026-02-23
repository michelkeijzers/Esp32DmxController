#pragma once
#include "../../main/Tasks/dmx_preset_changer.hpp"
#include <gmock/gmock.h>
class MockPresetChanger : public DmxPresetChanger
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

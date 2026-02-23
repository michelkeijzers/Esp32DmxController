#pragma once
#include "../../main/Tasks/foot_switch.hpp"
#include <gmock/gmock.h>
class MockFootSwitch : public FootSwitch
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, gpio_num_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

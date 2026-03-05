#pragma once
#include "../../main/Tasks/foot_switch.hpp"
#include <gmock/gmock.h>
// TU-local dummy Configuration used to satisfy FootSwitch base ctor
static Configuration _unit_test_dummy_configuration_foot_switch;

class MockFootSwitch : public FootSwitch
{
  public:
    MockFootSwitch(IAssert* assert, Configuration& config = _unit_test_dummy_configuration_foot_switch)
        : FootSwitch(assert, config) {}
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, gpio_num_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

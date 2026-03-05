#pragma once
#include "../../main/Tasks/nv_storage.hpp"
#include <gmock/gmock.h>
// TU-local dummy Configuration and DmxPresets used to satisfy NvStorage base ctor
static Configuration _unit_test_dummy_configuration_nv_storage;
static DmxPresets _unit_test_dummy_dmx_presets_nv_storage;

class MockNvStorage : public NvStorage
{
  public:
    MockNvStorage() : NvStorage(_unit_test_dummy_configuration_nv_storage, _unit_test_dummy_dmx_presets_nv_storage) {}
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

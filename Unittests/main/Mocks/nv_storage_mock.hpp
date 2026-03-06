#pragma once
#include "../../main/Tasks/nv_storage.hpp"
#include <gmock/gmock.h>
// TU-local dummy Configuration and DmxPresets used to satisfy NvStorage base ctor
#include "mock_assert.hpp"
static MockAssert _unit_test_mock_assert_nv_storage;
static Configuration _unit_test_dummy_configuration_nv_storage(&_unit_test_mock_assert_nv_storage);
static DmxPresets _unit_test_dummy_dmx_presets_nv_storage(&_unit_test_mock_assert_nv_storage);

class MockNvStorage : public NvStorage
{
  public:
    MockNvStorage(IAssert* assert, Configuration& config = _unit_test_dummy_configuration_nv_storage, DmxPresets& presets = _unit_test_dummy_dmx_presets_nv_storage)
        : NvStorage(assert, config, presets) {}
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

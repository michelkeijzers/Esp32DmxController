#pragma once
#include "../../main/Tasks/nvs_storage.hpp"
#include <gmock/gmock.h>
class MockNvsStorage : public NvsStorage
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

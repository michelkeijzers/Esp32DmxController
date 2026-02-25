#pragma once
#include "../../main/Tasks/nv_storage.hpp"
#include <gmock/gmock.h>
class MockNvStorage : public NvStorage
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

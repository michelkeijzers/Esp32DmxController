#pragma once
#include "max3485_sender.hpp"
#include <gmock/gmock.h>

class MockMax3485Sender : public Max3485Sender
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t dmxControllerEventQueue), (override));
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t dmxControllerEventQueue, const char *dummy, uint16_t dummy2), ());
    MOCK_METHOD(void, close, (), ());
    MOCK_METHOD(esp_err_t, sendDmx, (const uint8_t *data, uint16_t length), ());
    MOCK_METHOD(void, taskEntry, (void *param), (override));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), (const, override));
};

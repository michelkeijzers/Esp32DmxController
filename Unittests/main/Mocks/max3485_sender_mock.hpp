#pragma once
#include "../../main/Tasks/max3485_sender.hpp"
#include <gmock/gmock.h>

class MockMax3485Sender : public Max3485Sender
{
  public:
    MockMax3485Sender(IAssert *assert) : Max3485Sender(assert) {}
    MOCK_METHOD(void, init, (RtosTask::TaskProperties taskProperties), (override));
    MOCK_METHOD(void, init, (QueueHandle_t dmxControllerEventQueue, const char *dummy, uint16_t dummy2), ());
    MOCK_METHOD(void, close, (), ());
    MOCK_METHOD(void, sendDmx, (const uint8_t *data, uint16_t length), ());
    MOCK_METHOD(void, taskEntry, (void *param), (override));
    MOCK_METHOD(QueueHandle_t, getEventQueue, (), (const, override));
};

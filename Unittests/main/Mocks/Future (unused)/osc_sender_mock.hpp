#pragma once
#include "../../main/Tasks/osc_sender.hpp"
#include <gmock/gmock.h>
class MockOSCSender : public OSCSender
{
  public:
    MOCK_METHOD(esp_err_t, init, (const char *, uint16_t));
};

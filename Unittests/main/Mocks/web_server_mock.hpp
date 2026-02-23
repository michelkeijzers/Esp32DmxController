#pragma once
#include "../../main/Tasks/web_server.hpp"
#include <gmock/gmock.h>
class MockWebServer : public WebServer
{
  public:
    MOCK_METHOD(esp_err_t, init, ());
};

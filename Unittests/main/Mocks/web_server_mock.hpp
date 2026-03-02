#pragma once
#include "../../main/Tasks/web_server.hpp"
#include <gmock/gmock.h>

class MockWebServer : public WebServer
{
  public:
    MockWebServer(DmxPresets *presets = nullptr) : WebServer() { dmxPresets_ = presets; }
    MOCK_METHOD(esp_err_t, init, ());
    using WebServer::presets_to_json;
};

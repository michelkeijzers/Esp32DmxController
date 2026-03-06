#pragma once
#include "../../main/Tasks/web_server.hpp"
#include <gmock/gmock.h>

// TU-local dummy assert for WebServer
#include "mock_assert.hpp"
static MockAssert _unit_test_mock_assert_web_server;
class MockWebServer : public WebServer
{
  public:
    MockWebServer(DmxPresets *presets = nullptr)
        : WebServer(&_unit_test_mock_assert_web_server) { dmxPresets_ = presets; }
    MOCK_METHOD(esp_err_t, init, ());
    using WebServer::presets_to_json;
};

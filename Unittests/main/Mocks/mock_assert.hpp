#pragma once
#include "../../main/Base/assert.hpp"
#include <gmock/gmock.h>

class MockAssert : public IAssert {
public:
    MOCK_METHOD(void, assertNotEspError, (esp_err_t result, const char *message), (override));
    MOCK_METHOD(void, assertPdPass, (BaseType_t result, const char *message), (override));
    MOCK_METHOD(void, assertQueueHandle, (QueueHandle_t queue, const char *queueName), (override));
    MOCK_METHOD(void, assertNvsHandle, (nvs_handle_t handle, const char *handleName), (override));
    MOCK_METHOD(void, assertNotNull, (const void *ptr, const char *variableName), (override));
    MOCK_METHOD(void, assertNot0, (int handle, const char *variableName), (override));
    MOCK_METHOD(void, assertTrue, (bool variable, const char *variableName), (override));
    MOCK_METHOD(void, assertSoftwareError, (const char *message), (override));
    MOCK_METHOD(void, Halt, (), (override));
};

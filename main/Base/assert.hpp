#pragma once
#include <freertos/FreeRTOS.h>
#include <nvs.h>

class Assert
{
  public:
    static void assertPdPass(BaseType_t result, const char *message);
    static void assertQueueHandle(QueueHandle_t queue, const char *queueName);
    static void assertNotNull(const void *ptr, const char *variableName);
    static void assertNot0(int handle, const char *variableName);
    static void assertTrue(bool variable, const char *variableName);
    static void assertSoftwareError(const char *message);
};

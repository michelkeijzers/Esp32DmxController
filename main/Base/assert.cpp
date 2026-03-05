#include "assert.hpp"
#include <cstdio>
#include <cstdlib>
#include <esp_log.h>

void Assert::assertPdPass(BaseType_t result, const char *message)
{
    if (result != pdPASS)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "pdPASS assertion failed: %s", message);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void Assert::assertQueueHandle(QueueHandle_t queue, const char *queueName)
{
    if (queue == nullptr)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is 0", queueName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void Assert::assertNotNull(const void *ptr, const char *variableName)
{
    if (ptr == nullptr)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is null", variableName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void Assert::assertNot0(int handle, const char *variableName)
{
    if (handle == 0)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is 0", variableName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void Assert::assertTrue(bool variable, const char *variableName)
{
    if (!variable)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is false", variableName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void Assert::assertSoftwareError(const char *message)
{
    ESP_LOGE(pcTaskGetName(nullptr), "Software error: %s", message);
    while (true)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

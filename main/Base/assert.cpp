#include "assert.hpp"
#include <cstdio>
#include <cstdlib>
#include <esp_log.h>

void Assert::assertNotEspError(esp_err_t result, const char *message)
{
    if (result != ESP_OK)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "ESP error: %s, code: %d", message, result);
        Assert::Halt();
    }
}

void Assert::assertPdPass(BaseType_t result, const char *message)
{
    if (result != pdPASS)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "pdPASS assertion failed: %s", message);
        Assert::Halt();
    }
}

void Assert::assertQueueHandle(QueueHandle_t queue, const char *queueName)
{
    if (queue == nullptr)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "Event queue %s is 0", queueName);
        Assert::Halt();
    }
}

void Assert::assertNvsHandle(nvs_handle_t handle, const char *handleName)
{
    if (handle == 0)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "Handle %s is 0", handleName);
        Assert::Halt();
    }
}

void Assert::assertNotNull(const void *ptr, const char *variableName)
{
    if (ptr == nullptr)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is null", variableName);
        Assert::Halt();
    }
}

void Assert::assertNot0(int handle, const char *variableName)
{
    if (handle == 0)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is 0", variableName);
        Assert::Halt();
    }
}

void Assert::assertTrue(bool variable, const char *variableName)
{
    if (!variable)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "%s is false", variableName);
        Assert::Halt();
    }
}

void Assert::assertSoftwareError(const char *message)
{
    ESP_LOGE(pcTaskGetName(nullptr), "Software error: %s", message);
    Halt();
}

void Assert::Halt()
{
    while (true)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

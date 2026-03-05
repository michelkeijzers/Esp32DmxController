
#pragma once
#include <freertos/FreeRTOS.h>
#include <nvs.h>

class IAssert
{
  public:
    virtual void assertNotEspError(esp_err_t result, const char *message) = 0;
    virtual void assertPdPass(BaseType_t result, const char *message) = 0;
    virtual void assertQueueHandle(QueueHandle_t queue, const char *queueName) = 0;
    virtual void assertNvsHandle(nvs_handle_t handle, const char *handleName) = 0;
    virtual void assertNotNull(const void *ptr, const char *variableName) = 0;
    virtual void assertNot0(int handle, const char *variableName) = 0;
    virtual void assertTrue(bool variable, const char *variableName) = 0;
    virtual void assertSoftwareError(const char *message) = 0;
    virtual void Halt() = 0;
    virtual ~IAssert() = default;
};

class Assert : public IAssert
{
  public:
    void assertNotEspError(esp_err_t result, const char *message) override;
    void assertPdPass(BaseType_t result, const char *message) override;
    void assertQueueHandle(QueueHandle_t queue, const char *queueName) override;
    void assertNvsHandle(nvs_handle_t handle, const char *handleName) override;
    void assertNotNull(const void *ptr, const char *variableName) override;
    void assertNot0(int handle, const char *variableName) override;
    void assertTrue(bool variable, const char *variableName) override;
    void assertSoftwareError(const char *message) override;
    void Halt() override;
};

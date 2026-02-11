#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_err.h>

class RtosTask
{
protected:
    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;
    bool initialized_;
    const char *taskName_;

public:
    RtosTask();
    virtual ~RtosTask();

    esp_err_t init(const char *taskName, uint32_t stackSize, UBaseType_t priority, size_t queueCapacity, size_t queueItemSize);
    virtual void taskEntry(void *param) = 0;
    TaskHandle_t getTaskHandle() const { return taskHandle_; }
    QueueHandle_t getEventQueue() const { return eventQueue_; }
    const char *getTaskName() const { return taskName_; }
    bool isInitialized() const { return initialized_; }
};

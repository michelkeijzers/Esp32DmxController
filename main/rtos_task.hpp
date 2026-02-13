#pragma once
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

class RtosTask {
  protected:
    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;
    QueueHandle_t dmxControllerEventQueue_;
    bool initialized_;
    const char *taskName_;

  public:
    RtosTask();
    virtual ~RtosTask();

    esp_err_t init(const char *taskName, uint32_t stackSize, UBaseType_t priority, size_t queueCapacity,
        size_t queueItemSize, QueueHandle_t &dmxControllerQueue);
    virtual void taskEntry(void *param) = 0;
    TaskHandle_t getTaskHandle() const { return taskHandle_; }
    QueueHandle_t getEventQueue() const { return eventQueue_; }
    QueueHandle_t getDmxControllerEventQueue() const { return dmxControllerEventQueue_; }
    const char *getTaskName() const { return taskName_; }
    bool isInitialized() const { return initialized_; }
};

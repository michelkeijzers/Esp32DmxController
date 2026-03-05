#pragma once
#include <cstdint>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <nvs.h>

class RtosTask
{
  public:
    struct TaskProperties
    {
        const char *taskName_;
        UBaseType_t taskPriority;
        size_t stackSize;
        size_t queueCapacity;
        size_t queueItemSize;
        QueueHandle_t mainEventQueue;
    };

  protected:
    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;
    bool initialized_;
    QueueHandle_t mainEventQueue_;

  public:
    RtosTask();
    virtual ~RtosTask();

    void setMainEventQueue(QueueHandle_t queue) { mainEventQueue_ = queue; }
    TaskProperties CreateTaskProperties(
        const char *taskName, UBaseType_t taskPriority, size_t stackSize, size_t queueCapacity, size_t queueItemSize);
    void init(TaskProperties taskProperties);
    virtual void taskEntry(void *param) = 0;
    TaskHandle_t getTaskHandle() const { return taskHandle_; }
    virtual QueueHandle_t getEventQueue() const { return eventQueue_; }
    QueueHandle_t getDmxControllerEventQueue() const { return mainEventQueue_; }
    bool isInitialized() const { return initialized_; }
};

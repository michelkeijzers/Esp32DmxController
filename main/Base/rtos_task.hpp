#pragma once
#include <cstdint>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

class RtosTask
{
  public:
    struct TaskProperties
    {
        const char *taskName_;
        const char *logTag;
        UBaseType_t taskPriority;
        size_t stackSize;
        size_t queueCapacity;
        size_t queueItemSize;
        QueueHandle_t mainEventQueue;
    };

  protected:
    const char *task_name;
    const char *log_tag_;
    TaskHandle_t task_handle_;
    QueueHandle_t event_queue_;
    bool initialized_;
    QueueHandle_t main_event_queue_;

  public:
    RtosTask();
    virtual ~RtosTask();

    esp_err_t init(TaskProperties taskProperties);
    virtual void taskEntry(void *param) = 0;
    TaskHandle_t getTaskHandle() const { return task_handle_; }
    virtual QueueHandle_t getEventQueue() const { return event_queue_; }
    QueueHandle_t getDmxControllerEventQueue() const { return main_event_queue_; }
    const char *getTaskName() const { return task_name; }
    bool isInitialized() const { return initialized_; }
};

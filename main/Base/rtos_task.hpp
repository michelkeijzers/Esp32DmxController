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
        const char *logTag;
        UBaseType_t taskPriority;
        size_t stackSize;
        size_t queueCapacity;
        size_t queueItemSize;
        QueueHandle_t mainEventQueue;
    };

  protected:
    const char *task_name_;
    const char *log_tag_;
    TaskHandle_t task_handle_;
    QueueHandle_t event_queue_;
    bool initialized_;
    QueueHandle_t main_event_queue_;

  public:
    RtosTask();
    virtual ~RtosTask();

    void setMainEventQueue(QueueHandle_t queue) { main_event_queue_ = queue; }
    TaskProperties CreateTaskProperties(const char *taskName, const char *logTag, UBaseType_t taskPriority,
        size_t stackSize, size_t queueCapacity, size_t queueItemSize);
    void init(TaskProperties taskProperties);
    virtual void taskEntry(void *param) = 0;
    TaskHandle_t getTaskHandle() const { return task_handle_; }
    virtual QueueHandle_t getEventQueue() const { return event_queue_; }
    QueueHandle_t getDmxControllerEventQueue() const { return main_event_queue_; }
    const char *getTaskName() const { return task_name_; }
    bool isInitialized() const { return initialized_; }
    void assertNotNull(const void *ptr, const char *variableName);
    void assertNot0(nvs_handle_t handle, const char *variableName); // for handles
    void assertTrue(bool variable, const char *variableName);
    void softwareError(const char *message);
};

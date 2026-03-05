#include "rtos_task.hpp"
#include "esp_err.h"
#include "esp_log.h"

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
}
#include <stdio.h>

RtosTask::RtosTask() : taskHandle_(nullptr), eventQueue_(nullptr), initialized_(false) {}

RtosTask::~RtosTask()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    // Only delete eventQueue_ if it is not a stub (0x1 or 0xDEADBEEF)
    if (eventQueue_ && eventQueue_ != reinterpret_cast<QueueHandle_t>(0x1) &&
        eventQueue_ != reinterpret_cast<QueueHandle_t>(static_cast<uintptr_t>(0xDEADBEEF)))
    {
        vQueueDelete(eventQueue_);
    }
}

RtosTask::TaskProperties RtosTask::CreateTaskProperties(
    const char *taskName, UBaseType_t taskPriority, size_t stackSize, size_t queueCapacity, size_t queueItemSize)
{
    TaskProperties taskProperties;
    taskProperties.taskName_ = taskName;
    taskProperties.taskPriority = taskPriority;
    taskProperties.stackSize = stackSize;
    taskProperties.queueCapacity = queueCapacity;
    taskProperties.queueItemSize = queueItemSize;
    taskProperties.mainEventQueue = mainEventQueue_;
    return taskProperties;
}

void RtosTask::init(TaskProperties taskProperties)
{
    mainEventQueue_ = taskProperties.mainEventQueue;

    eventQueue_ = xQueueCreate(
        static_cast<uint32_t>(taskProperties.queueCapacity), static_cast<uint32_t>(taskProperties.queueItemSize));
    if (eventQueue_)
    {
        ESP_LOGI(pcTaskGetName(nullptr), "Event queue creation successfully");
    }
    else
    {
        ESP_LOGE(pcTaskGetName(nullptr), "Failed to create event queue");
        return;
    }

    // Static entry wrapper
    auto entry = [](void *param) { static_cast<RtosTask *>(param)->taskEntry(param); };

    if (xTaskCreate(entry, taskProperties.taskName_, static_cast<uint32_t>(taskProperties.stackSize), this,
            taskProperties.taskPriority, &taskHandle_) == pdPASS)
    {
        ESP_LOGI(pcTaskGetName(nullptr), "Task created successfully");
    }
    else
    {
        ESP_LOGE(pcTaskGetName(nullptr), "Failed to create task");
        vQueueDelete(eventQueue_);
        eventQueue_ = nullptr;
    }
    initialized_ = true;
}

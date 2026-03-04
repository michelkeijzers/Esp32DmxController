#include "rtos_task.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stdio.h>

RtosTask::RtosTask() : task_handle_(nullptr), event_queue_(nullptr), initialized_(false) {}

RtosTask::~RtosTask()
{
    if (task_handle_)
    {
        vTaskDelete(task_handle_);
    }
    // Only delete eventQueue_ if it is not a stub (0x1 or 0xDEADBEEF)
    if (event_queue_ && event_queue_ != reinterpret_cast<QueueHandle_t>(0x1) &&
        event_queue_ != reinterpret_cast<QueueHandle_t>(static_cast<uintptr_t>(0xDEADBEEF)))
    {
        vQueueDelete(event_queue_);
    }
}

RtosTask::TaskProperties RtosTask::CreateTaskProperties(const char *taskName, const char *logTag,
    UBaseType_t taskPriority, size_t stackSize, size_t queueCapacity, size_t queueItemSize)
{
    TaskProperties taskProperties;
    taskProperties.taskName_ = taskName;
    taskProperties.logTag = logTag;
    taskProperties.taskPriority = taskPriority;
    taskProperties.stackSize = stackSize;
    taskProperties.queueCapacity = queueCapacity;
    taskProperties.queueItemSize = queueItemSize;
    taskProperties.mainEventQueue = main_event_queue_;
    return taskProperties;
}

void RtosTask::init(TaskProperties taskProperties)
{
    task_name_ = taskProperties.taskName_;
    log_tag_ = taskProperties.logTag;
    main_event_queue_ = taskProperties.mainEventQueue;

    event_queue_ = xQueueCreate(
        static_cast<uint32_t>(taskProperties.queueCapacity), static_cast<uint32_t>(taskProperties.queueItemSize));
    if (event_queue_)
    {
        ESP_LOGI(log_tag_, "Event queue creation successfully");
    }
    else
    {
        ESP_LOGE(log_tag_, "Failed to create event queue");
        return;
    }

    // Static entry wrapper
    auto entry = [](void *param) { static_cast<RtosTask *>(param)->taskEntry(param); };

    if (xTaskCreate(entry, taskProperties.taskName_, static_cast<uint32_t>(taskProperties.stackSize), this,
            taskProperties.taskPriority, &task_handle_) == pdPASS)
    {
        ESP_LOGI(log_tag_, "Task created successfully");
    }
    else
    {
        ESP_LOGE(log_tag_, "Failed to create task");
        vQueueDelete(event_queue_);
        event_queue_ = nullptr;
    }
    initialized_ = true;
}

void RtosTask::assertNotNull(const void *ptr, const char *variableName)
{
    if (ptr == nullptr)
    {
        ESP_LOGE(log_tag_, "%s: %s is null", task_name_, variableName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void RtosTask::assertNot0(nvs_handle_t handle, const char *variableName)
{
    if (handle == 0)
    {
        ESP_LOGE(log_tag_, "%s: %s is 0", task_name_, variableName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void RtosTask::assertTrue(bool variable, const char *variableName)
{
    if (!variable)
    {
        ESP_LOGE(log_tag_, "%s: %s is false", task_name_, variableName);
        while (true)
        {
            vTaskDelay(portMAX_DELAY);
        }
    }
}

void RtosTask::softwareError(const char *message)
{
    ESP_LOGE(log_tag_, "%s: Software error: %s", task_name_, message);
    while (true)
    {
        vTaskDelay(portMAX_DELAY);
    }
}

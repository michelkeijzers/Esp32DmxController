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
    // Only delete eventQueue_ if it is not the test stub dummyQueue (0x1)
    if (event_queue_ && event_queue_ != reinterpret_cast<QueueHandle_t>(0x1))
    {
        vQueueDelete(event_queue_);
    }
}

esp_err_t RtosTask::init(TaskProperties taskProperties)
{
    task_name = taskProperties.taskName_;
    log_tag_ = taskProperties.logTag;
    main_event_queue_ = taskProperties.mainEventQueue;

    event_queue_ = xQueueCreate(
        static_cast<uint32_t>(taskProperties.queueCapacity), static_cast<uint32_t>(taskProperties.queueItemSize));
    if (!event_queue_)
    {
        ESP_LOGI(log_tag_, "Event queue creation successfully");
    }
    else
    {
        ESP_LOGE(log_tag_, "Failed to create event queue");
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
        return ESP_FAIL;
    }
    initialized_ = true;
    return ESP_OK;
}

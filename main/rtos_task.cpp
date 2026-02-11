#include "rtos_task.hpp"
#include "esp_log.h"

RtosTask::RtosTask()
    : taskHandle_(nullptr),
      eventQueue_(nullptr),
      initialized_(false)
{
}

RtosTask::~RtosTask()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    if (eventQueue_)
    {
        vQueueDelete(eventQueue_);
    }
}

esp_err_t RtosTask::init(const char *taskName, uint32_t stackSize, UBaseType_t priority, size_t queueCapacity, size_t queueItemSize)
{
    taskName_ = taskName;

    eventQueue_ = xQueueCreate(queueCapacity, queueItemSize);
    if (!eventQueue_)
    {
        ESP_LOGI(taskName_, "Event queue creation successfully");
    }
    else
    {
        ESP_LOGE(taskName_, "Failed to create event queue");
    }

    // Static entry wrapper
    auto entry = [](void *param)
    {
        static_cast<RtosTask *>(param)->taskEntry(param);
    };

    if (xTaskCreate(entry, taskName, stackSize, this, priority, &taskHandle_) == pdPASS)
    {
        ESP_LOGI(taskName, "Task created successfully");
    }
    else
    {
        ESP_LOGE(taskName, "Failed to create task");
        vQueueDelete(eventQueue_);
        eventQueue_ = nullptr;
        return ESP_FAIL;
    }

    initialized_ = true;
    return ESP_OK;
}

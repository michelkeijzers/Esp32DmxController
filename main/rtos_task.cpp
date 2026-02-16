#include "rtos_task.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stdio.h>

RtosTask::RtosTask() : taskHandle_(nullptr), eventQueue_(nullptr), initialized_(false) {}

RtosTask::~RtosTask()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    // Only delete eventQueue_ if it is not the test stub dummyQueue (0x1)
    if (eventQueue_ && eventQueue_ != reinterpret_cast<QueueHandle_t>(0x1))
    {
        vQueueDelete(eventQueue_);
    }
}

esp_err_t RtosTask::init(const char *taskName, uint32_t stackSize, UBaseType_t priority, size_t queueCapacity,
    size_t queueItemSize, QueueHandle_t &dmxControllerEventQueue)
{
    printf("Initializing task: %s\n", taskName);
    taskName_ = taskName;
    dmxControllerEventQueue_ = dmxControllerEventQueue;
    printf("Task %s: dmxControllerEventQueue handle = %p\n", taskName, (void *)dmxControllerEventQueue_);

    eventQueue_ = xQueueCreate(static_cast<unsigned int>(queueCapacity), static_cast<unsigned int>(queueItemSize));
    if (!eventQueue_)
    {
        printf("Failed to create event queue for task %s\n", taskName);
        ESP_LOGI(taskName_, "Event queue creation successfully");
    }
    else
    {
        printf("Event queue created successfully for task %s, handle = %p\n", taskName, (void *)eventQueue_);
        ESP_LOGE(taskName_, "Failed to create event queue");
    }

    // Static entry wrapper
    printf("Creating task %s...\n", taskName);
    auto entry = [](void *param) { static_cast<RtosTask *>(param)->taskEntry(param); };

    printf("Starting task %s...\n", taskName);
    if (xTaskCreate(entry, taskName, stackSize, this, priority, &taskHandle_) == pdPASS)
    {
        printf("Task %s created successfully, handle = %p\n", taskName, (void *)taskHandle_);
        ESP_LOGI(taskName, "Task created successfully");
    }
    else
    {
        printf("Failed to create task %s\n", taskName);
        ESP_LOGE(taskName, "Failed to create task");
        vQueueDelete(eventQueue_);
        eventQueue_ = nullptr;
        return ESP_FAIL;
    }
    printf("Task %s initialized successfully\n", taskName);
    initialized_ = true;
    return ESP_OK;
}

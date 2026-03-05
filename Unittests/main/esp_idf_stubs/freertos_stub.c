#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // FreeRTOS constants
    const BaseType_t pdPASS = 1;
    const BaseType_t pdTRUE = 1;
    const BaseType_t pdFALSE = 0;
    const TickType_t portMAX_DELAY = 0xFFFFFFFF;

    // Allow test override of xQueueCreate via function pointer
    typedef void *(*xQueueCreate_fn)(unsigned int, unsigned int);
    // Use weak linkage or extern for test pointer
    __attribute__((weak)) xQueueCreate_fn xQueueCreate_ptr = 0;
    QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize)
    {
        if (xQueueCreate_ptr)
            return xQueueCreate_ptr(uxQueueLength, uxItemSize);
        return (QueueHandle_t)0x1;
    }
    void vQueueDelete(QueueHandle_t xQueue) {}
    BaseType_t xQueueSend(QueueHandle_t xQueue, const void *item, TickType_t ticks) { return pdPASS; }
    BaseType_t xQueueReceive(QueueHandle_t xQueue, void *item, TickType_t ticks) { return pdFALSE; }
    BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *item, BaseType_t *pxHigherPriorityTaskWoken)
    {
        return pdPASS;
    }

    // Stubs for semaphore functions
    SemaphoreHandle_t xSemaphoreCreateMutex(void) { return (SemaphoreHandle_t)0x2; }
    void vSemaphoreDelete(SemaphoreHandle_t xSemaphore) {}
    BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t ticks) { return pdTRUE; }
    BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) { return pdTRUE; }

    // Stubs for task functions
    BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char *const pcName, uint32_t usStackDepth,
        void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask)
    {
        if (pxCreatedTask)
            *pxCreatedTask = (TaskHandle_t)0x3;
        return pdPASS;
    }
    void vTaskDelete(TaskHandle_t xTaskToDelete) {}
    void vTaskDelay(const TickType_t xTicksToDelay) {}
    TickType_t xTaskGetTickCount(void)
    {
        static TickType_t tick = 0;
        return ++tick;
    }

    // Conversion macro stub
    TickType_t pdMS_TO_TICKS(uint32_t ms) { return ms; }

#ifdef __cplusplus
}
#endif

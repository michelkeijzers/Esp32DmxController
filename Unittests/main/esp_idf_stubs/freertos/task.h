#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
// Minimal FreeRTOS task stub for unit testing
#include "FreeRTOS.h"

    static inline TickType_t xTaskGetTickCount(void) { return 0; }
    static inline void vTaskDelay(TickType_t) {}

    static inline void vTaskDelete(void *) {}
    typedef void (*TaskFunction_t)(void *);

    typedef void *TaskHandle_t;
#define pdPASS 1

    // Match the real FreeRTOS signature for xTaskCreate
    int xTaskCreate(TaskFunction_t pxTaskCode, const char *const pcName, uint32_t usStackDepth, void *pvParameters,
        uint32_t uxPriority, TaskHandle_t *pxCreatedTask);

#ifdef __cplusplus
}
#endif

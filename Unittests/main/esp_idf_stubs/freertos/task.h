#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
// Minimal FreeRTOS task stub for unit testing
#include "FreeRTOS.h"

#include <stdint.h>
    static inline TickType_t xTaskGetTickCount(void) { return 0; }
    static inline TickType_t xTaskGetTickCountFromISR(void) { return 0; }
    static inline void vTaskDelay(TickType_t) {}
    static inline void vTaskDelete(void *) {}
    typedef void (*TaskFunction_t)(void *);
    typedef void *TaskHandle_t;
#define pdPASS 1
// --- Task name mapping for unit test stubs ---
#ifdef __cplusplus
    extern "C"
    {
#endif
        int xTaskCreate(TaskFunction_t pxTaskCode, const char *const pcName, uint32_t usStackDepth, void *pvParameters,
            uint32_t uxPriority, TaskHandle_t *pxCreatedTask);
        const char *pcTaskGetName(TaskHandle_t handle);
#ifdef __cplusplus
    }
#endif
#ifdef __cplusplus
}
#endif

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
// Minimal FreeRTOS stub for unit testing
#define pdPASS 1
#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFF
#ifndef portBASE_TYPE
#define portBASE_TYPE unsigned int
#endif
#ifndef UBaseType_t
#define UBaseType_t unsigned int
#endif
#ifndef portTICK_PERIOD_MS
#define portTICK_PERIOD_MS 1
#endif
    typedef void *QueueHandle_t;
    typedef unsigned int TickType_t;
    typedef void *TaskHandle_t;
    typedef int BaseType_t;
#define pdMS_TO_TICKS(ms) (ms)
#ifndef portYIELD_FROM_ISR
#define portYIELD_FROM_ISR() ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#pragma once

// Minimal stub for FreeRTOS semphr.h for unit testing on host

#include <stdint.h>

typedef void *SemaphoreHandle_t;

typedef int32_t BaseType_t;
#define UBaseType_t unsigned int
typedef uint32_t TickType_t;

// Stub functions
static inline SemaphoreHandle_t xSemaphoreCreateMutex(void) { return (SemaphoreHandle_t)1; }
static inline int xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait)
{
    (void)xSemaphore;
    (void)xTicksToWait;
    return 1;
}
static inline int xSemaphoreGive(SemaphoreHandle_t) { return 1; }
static inline void vSemaphoreDelete(SemaphoreHandle_t) {}

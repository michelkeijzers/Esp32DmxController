#pragma once

typedef void *QueueHandle_t;

#ifdef __cplusplus
extern "C"
{
#endif
// Minimal FreeRTOS queue stub for unit testing
#include "FreeRTOS.h"
#ifndef portBASE_TYPE
#define portBASE_TYPE unsigned int
#endif
    QueueHandle_t xQueueCreate(portBASE_TYPE, portBASE_TYPE);
    int xQueueSend(QueueHandle_t, const void *, unsigned int);
    int xQueueReceive(QueueHandle_t, void *, unsigned int); // Implemented in test file for test control
    void vQueueDelete(void *);
    int xQueueSendFromISR(void *, const void *, int *);
#ifdef __cplusplus
}
#endif

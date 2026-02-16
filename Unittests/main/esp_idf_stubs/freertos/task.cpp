#include "task.h"
#include <stdint.h>

// You can add counters or flags here for test verification if needed
extern "C" int xTaskCreate(TaskFunction_t pxTaskCode, const char *const pcName, uint32_t usStackDepth,
    void *pvParameters, uint32_t uxPriority, TaskHandle_t *pxCreatedTask)
{
    // Simulate successful task creation
    return pdPASS;
}

#include "freertos/task.h"
#include <string.h>

#define MAX_TASKS 16
typedef struct
{
    TaskHandle_t handle;
    char name[32];
} TaskStubEntry;
static TaskStubEntry g_taskStubTable[MAX_TASKS];
static int g_taskStubCount = 0;

int xTaskCreate(TaskFunction_t pxTaskCode, const char *const pcName, uint32_t usStackDepth, void *pvParameters,
    uint32_t uxPriority, TaskHandle_t *pxCreatedTask)
{
    if (g_taskStubCount >= MAX_TASKS)
        return 0;
    TaskStubEntry *entry = &g_taskStubTable[g_taskStubCount++];
    entry->handle = (TaskHandle_t)entry;
#if defined(_MSC_VER)
    strncpy_s(entry->name, sizeof(entry->name), pcName ? pcName : "mock_task", _TRUNCATE);
#else
    strncpy(entry->name, pcName ? pcName : "mock_task", sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';
#endif
    if (pxCreatedTask)
        *pxCreatedTask = entry->handle;
    // Optionally call the function (not needed for stub)
    return 1;
}

const char *pcTaskGetName(TaskHandle_t handle)
{
    if (handle == NULL && g_taskStubCount > 0)
    {
        // Return the name of the most recently created task
        return g_taskStubTable[g_taskStubCount - 1].name;
    }
    for (int i = 0; i < g_taskStubCount; ++i)
    {
        if (g_taskStubTable[i].handle == handle)
        {
            return g_taskStubTable[i].name;
        }
    }
    return "mock_task";
}
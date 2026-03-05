#include "freertos/queue.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize)
    {
        // Always return a non-null dummy value for host tests
        ESP_LOGI("xq", "1");
        return (QueueHandle_t)0x1;
    }

    void vQueueDelete(QueueHandle_t xQueue)
    {
        // No-op for host tests
    }

#ifdef __cplusplus
}
#endif

#include "freertos/queue.h"
// All stub definitions outside extern "C" to avoid MSVC C4114 warning

QueueHandle_t (*xQueueCreate_ptr)(portBASE_TYPE, portBASE_TYPE) = nullptr;
int (*xQueueSend_ptr)(QueueHandle_t, const void *, unsigned int) = nullptr;
int (*xQueueReceive_ptr)(QueueHandle_t, void *, unsigned int) = nullptr;
void (*vQueueDelete_ptr)(void *) = nullptr;
int (*xQueueSendFromISR_ptr)(void *, const void *, int *) = nullptr;
#include "freertos/queue.h"
// All stub definitions outside extern "C" to avoid MSVC C4114 warning
// ...existing code...

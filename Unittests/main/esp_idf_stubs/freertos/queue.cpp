#include "freertos/queue.h"
// All stub definitions outside extern "C" to avoid MSVC C4114 warning
QueueHandle_t xQueueCreate(portBASE_TYPE, portBASE_TYPE) { return (QueueHandle_t)1; }
int xQueueSend(QueueHandle_t, const void *, unsigned int) { return 1; }
void vQueueDelete(void *) {}
int xQueueSendFromISR(void *, const void *, int *) { return 0; }
#include "freertos/queue.h"
#include "messages.hpp"

int xQueueReceive(QueueHandle_t, void *pvBuffer, unsigned int)
{
    using MessagesEvent = Messages::Event;
    static int call_count = 0;
    call_count++;
    if (!pvBuffer)
        return 0;
    MessagesEvent *event = static_cast<MessagesEvent *>(pvBuffer);
    // Simulate the sequence in init_messages:
    // 1st call: config response, 2nd call: presets response
    if (call_count == 1)
    {
        event->type = Messages::EventType::CONFIGURATION_RESPONSE;
    }
    else if (call_count == 2)
    {
        event->type = Messages::EventType::PRESETS_RESPONSE;
    }
    return 1; // pdTRUE
}

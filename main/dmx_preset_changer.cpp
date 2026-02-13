#include "dmx_preset_changer.hpp"
#include <esp_log.h>

static const char *LOG_TAG = "DmxPresetChanger";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

DmxPresetChanger::DmxPresetChanger() : RtosTask() {}

DmxPresetChanger::~DmxPresetChanger() {}

esp_err_t DmxPresetChanger::init(QueueHandle_t dmxControllerEventQueue) {
    if (RtosTask::init("DmxPresetChangerTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Event),
            dmxControllerEventQueue) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxPresetChangerTask");
        return ESP_FAIL;
    }

    ESP_LOGI(LOG_TAG, "DmxPresetChanger task started");
    return ESP_OK;
}

void DmxPresetChanger::taskEntry(void *param) { static_cast<DmxPresetChanger *>(param)->taskLoop(); }

void DmxPresetChanger::taskLoop() {
    EventType event;
    while (true) {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE) {
            switch (event) {
            case PREVIOUS_PRESET:
                // TODO
                break;
            case NEXT_PRESET:
                // TODO
                break;
            }
        }
    }
}

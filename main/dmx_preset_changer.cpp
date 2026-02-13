#include "dmx_preset_changer.hpp"
#include "messages.hpp"
#include <esp_log.h>

static const char *LOG_TAG = "DmxPresetChanger";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

DmxPresetChanger::DmxPresetChanger() : RtosTask() {}

DmxPresetChanger::~DmxPresetChanger() {}

esp_err_t DmxPresetChanger::init(QueueHandle_t dmxControllerEventQueue) {
    if (RtosTask::init("DmxPresetChangerTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Messages::Event),
            dmxControllerEventQueue) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxPresetChangerTask");
        return ESP_FAIL;
    }

    ESP_LOGI(LOG_TAG, "DmxPresetChanger task started");
    return ESP_OK;
}

void DmxPresetChanger::taskEntry(void *param) { static_cast<DmxPresetChanger *>(param)->taskLoop(); }

void DmxPresetChanger::taskLoop() {
    Messages::Event event;
    while (true) {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE) {
            switch (event.type) {
            case Messages::EventType::SET_PRESETS:
                setPresets(event.data.presetsData);
                break;

            default:
                // Ignore other events
                break;
            }
        }
    }
}

void DmxPresetChanger::setPresets(const Messages::PresetsEventData &presetsData) {
    dmxPresets_.clearAll();
    for (size_t i = 0; i < presetsData.number_of_presets; ++i) {
        dmxPresets_.addPreset(presetsData.presets[i].name, presetsData.presets[i].universe_1_length,
            presetsData.presets[i].universe_1_data, presetsData.presets[i].universe_2_length,
            presetsData.presets[i].universe_2_data);
    }
    ESP_LOGI(LOG_TAG, "Presets updated: number_of_presets=%d", dmxPresets_.getNumPresets());
}
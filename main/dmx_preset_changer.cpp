#include "dmx_preset_changer.hpp"
#include "messages.hpp"
#include <esp_log.h>

static const char *LOG_TAG = "DmxPresetChanger";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

DmxPresetChanger::DmxPresetChanger() : RtosTask() {}

DmxPresetChanger::~DmxPresetChanger() {}

esp_err_t DmxPresetChanger::init(QueueHandle_t dmxControllerEventQueue)
{
    if (RtosTask::init("DmxPresetChangerTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Messages::Event),
            dmxControllerEventQueue) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxPresetChangerTask");
        return ESP_FAIL;
    }

    ESP_LOGI(LOG_TAG, "DmxPresetChanger task started");
    return ESP_OK;
}

void DmxPresetChanger::taskEntry(void *param) { static_cast<DmxPresetChanger *>(param)->taskLoop(); }

void DmxPresetChanger::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event.type)
            {
            case Messages::EventType::SET_PRESETS:
                setPresets(event.data.presetsData);
                break;

            case Messages::EventType::SELECT_NEXT_PRESET:
            {
                dmxPresets_.selectNextPreset();
                Messages::Event dmxControllerEvent = Messages::Event();
                dmxControllerEvent.type = Messages::EventType::USE_PRESET_DATA;
                DmxPreset &currentPreset = dmxPresets_.getCurrentPreset();
                dmxControllerEvent.data.presetData.presetNumber = currentPreset.getIndex();
                dmxControllerEvent.data.presetData.name = currentPreset.getName();
                dmxControllerEvent.data.presetData.universe1Length = currentPreset.getUniverseLength(0);
                memcpy(dmxControllerEvent.data.presetData.universe1Data, currentPreset.getUniverseData(0),
                    dmxControllerEvent.data.presetData.universe1Length);
                dmxControllerEvent.data.presetData.universe2Length = currentPreset.getUniverseLength(1);
                memcpy(dmxControllerEvent.data.presetData.universe2Data, currentPreset.getUniverseData(1),
                    dmxControllerEvent.data.presetData.universe2Length);

                if (xQueueSend(getDmxControllerEventQueue(), &dmxControllerEvent, 0) != pdPASS)
                {
                    ESP_LOGE(LOG_TAG, "Failed to forward current preset data to DmxController");
                }
                ESP_LOGI(LOG_TAG, "Selected next preset: index=%d", dmxPresets_.getCurrentPresetIndex());
            }
            break;

            case Messages::EventType::SELECT_PREVIOUS_PRESET:
            {
                dmxPresets_.selectPreviousPreset();
                Messages::Event dmxControllerEvent = Messages::Event();
                dmxControllerEvent.type = Messages::EventType::USE_PRESET_DATA;
                DmxPreset &currentPreset = dmxPresets_.getCurrentPreset();
                dmxControllerEvent.data.presetData.presetNumber = currentPreset.getIndex();
                dmxControllerEvent.data.presetData.name = currentPreset.getName();
                dmxControllerEvent.data.presetData.universe1Length = currentPreset.getUniverseLength(0);
                memcpy(dmxControllerEvent.data.presetData.universe1Data, currentPreset.getUniverseData(0),
                    dmxControllerEvent.data.presetData.universe1Length);
                dmxControllerEvent.data.presetData.universe2Length = currentPreset.getUniverseLength(1);
                memcpy(dmxControllerEvent.data.presetData.universe2Data, currentPreset.getUniverseData(1),
                    dmxControllerEvent.data.presetData.universe2Length);

                if (xQueueSend(getDmxControllerEventQueue(), &dmxControllerEvent, 0) != pdPASS)
                {
                    ESP_LOGE(LOG_TAG, "Failed to forward current preset data to DmxController");
                }
                ESP_LOGI(LOG_TAG, "Selected previous preset: index=%d", dmxPresets_.getCurrentPresetIndex());
            }
            break;

            default:
                // Ignore other events
                break;
            }
        }
    }
}

void DmxPresetChanger::setPresets(const Messages::PresetsEventData &presetsData)
{
    dmxPresets_.clearAll();
    for (size_t i = 0; i < presetsData.numberOfPresets; ++i)
    {
        dmxPresets_.addPreset(presetsData.presets[i].presetNumber, presetsData.presets[i].name,
            presetsData.presets[i].universe1Length, presetsData.presets[i].universe1Data,
            presetsData.presets[i].universe2Length, presetsData.presets[i].universe2Data);
    }
    ESP_LOGI(LOG_TAG, "Presets updated: number of presets=%d", dmxPresets_.getNumPresets());
}
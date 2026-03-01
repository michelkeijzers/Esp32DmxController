#include "dmx_preset_changer.hpp"
#include "messages.hpp"
#include <esp_log.h>

DmxPresetChanger::DmxPresetChanger() {}

DmxPresetChanger::~DmxPresetChanger() {}

esp_err_t DmxPresetChanger::init(TaskProperties taskProperties)
{
    if (RtosTask::init(taskProperties) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize DmxPresetChangerTask");
        return ESP_FAIL;
    }

    ESP_LOGI(log_tag_, "DmxPresetChanger task started");
    return ESP_OK;
}

void DmxPresetChanger::taskEntry(void *param) { static_cast<DmxPresetChanger *>(param)->taskLoop(); }

void DmxPresetChanger::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) == pdTRUE)
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
                dmxControllerEvent.data.presetData.presetNumber = static_cast<uint8_t>(currentPreset.getIndex());
                dmxControllerEvent.data.presetData.name = currentPreset.getName();
                memcpy(dmxControllerEvent.data.presetData.dmxValues, currentPreset.getDmxValues(), NR_OF_DMX_CHANNELS);

                if (xQueueSend(getDmxControllerEventQueue(), &dmxControllerEvent, 0) != pdPASS)
                {
                    ESP_LOGE(log_tag_, "Failed to forward current preset data to DmxController");
                }
                ESP_LOGI(log_tag_, "Selected next preset: index=%d", dmxPresets_.getCurrentPresetIndex());
            }
            break;

            case Messages::EventType::SELECT_PREVIOUS_PRESET:
            {
                dmxPresets_.selectPreviousPreset();
                Messages::Event dmxControllerEvent = Messages::Event();
                dmxControllerEvent.type = Messages::EventType::USE_PRESET_DATA;
                DmxPreset &currentPreset = dmxPresets_.getCurrentPreset();
                dmxControllerEvent.data.presetData.presetNumber = static_cast<uint8_t>(currentPreset.getIndex());
                dmxControllerEvent.data.presetData.name = currentPreset.getName();
                memcpy(dmxControllerEvent.data.presetData.dmxValues, currentPreset.getDmxValues(), NR_OF_DMX_CHANNELS);

                if (xQueueSend(getDmxControllerEventQueue(), &dmxControllerEvent, 0) != pdPASS)
                {
                    ESP_LOGE(log_tag_, "Failed to forward current preset data to DmxController");
                }
                ESP_LOGI(log_tag_, "Selected previous preset: index=%d", dmxPresets_.getCurrentPresetIndex());
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
    for (uint8_t i = 0; i < static_cast<uint8_t>(presetsData.numberOfPresets); ++i)
    {
        dmxPresets_.addPreset(static_cast<uint8_t>(presetsData.presets[i].presetNumber), presetsData.presets[i].name,
            presetsData.presets[i].dmxValues);
    }
    ESP_LOGI(log_tag_, "Presets updated: number of presets=%d", dmxPresets_.getNumPresets());
}
#include "nv_storage.hpp"

#include "../Base/assert.hpp"
#include <cstring>
#include <esp_log.h>

NvStorage::NvStorage(Configuration &configuration, DmxPresets &dmxPresets)
    : RtosTask(), configurationNvsHandle_(0), presetsNvsHandle_(0), configurationNamespaceName_("configuration"),
      presetsNamespaceName_("presets"), configuration_(configuration), dmxPresets_(dmxPresets)
{
}

NvStorage::~NvStorage()
{
    if (configurationNvsHandle_ != 0)
    {
        nvs_close(configurationNvsHandle_);
    }

    if (presetsNvsHandle_ != 0)
    {
        nvs_close(presetsNvsHandle_);
    }
}

void NvStorage::init(RtosTask::TaskProperties taskProperties)
{
    RtosTask::init(taskProperties);
    ESP_ERROR_CHECK(nvs_open(configurationNamespaceName_, NVS_READWRITE, &configurationNvsHandle_));
    ESP_ERROR_CHECK(nvs_open(presetsNamespaceName_, NVS_READWRITE, &presetsNvsHandle_));
}

void NvStorage::taskEntry(void *param) { static_cast<NvStorage *>(param)->taskLoop(); }

void NvStorage::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(logTag_, "NvStorage event received: %d", event.type);
            switch (event.type)
            {
            case Messages::EventType::LOAD_CONFIGURATION:
                loadConfiguration();
                break;

            default:
                ESP_LOGW(logTag_, "Unknown NvStorage event type: %d", event.type);
                break;
            }
        }
    }
}

// void NvStorage::saveConfiguration(Configuration &configurationData)
// {
//     assertNot0(configurationNvsHandle_, "configurationNvsHandle_");
//     ESP_ERROR_CHECK(nvs_set_u8(
//         configurationNvsHandle_, "SwitchPolarityNormallyOpen", configurationData.switchPolarityNormallyOpen));
//     ESP_ERROR_CHECK(
//         nvs_set_u16(configurationNvsHandle_, "LongPressThreshold", configurationData.longPressThresholdMs));
//     ESP_ERROR_CHECK(nvs_commit(configurationNvsHandle_));
// }

void NvStorage::loadConfiguration()
{
    Assert::assertNot0(configurationNvsHandle_, "configurationNvsHandle_");

    configuration_.lock();

    uint8_t footSwitchPolarityNormallyOpen;
    ESP_ERROR_CHECK(
        nvs_get_u8(configurationNvsHandle_, "FootSwitchPolarityNormallyOpen", &footSwitchPolarityNormallyOpen));
    configuration_.setFootSwitchPolarityNormallyOpen(footSwitchPolarityNormallyOpen);

    uint16_t footSwitchLongPressTime_;
    ESP_ERROR_CHECK(nvs_get_u16(configurationNvsHandle_, "FootSwitchLongPressThreshold", &footSwitchLongPressTime_));
    configuration_.setFootSwitchLongPressTime(footSwitchLongPressTime_);

    uint8_t numberOfFilledPresets = 0;
    ESP_ERROR_CHECK(nvs_get_u8(configurationNvsHandle_, "NumberOfFilledPresets", &numberOfFilledPresets));
    configuration_.setNumberOfFilledPresets(numberOfFilledPresets);

    uint8_t circularPresetNavigation;
    ESP_ERROR_CHECK(nvs_get_u8(configurationNvsHandle_, "CircularPresetNavigation", &circularPresetNavigation));
    configuration_.setCircularPresetNavigation(circularPresetNavigation);

    configuration_.unlock();

    Messages::Event responseEvent;
    responseEvent.type = Messages::CONFIGURATION_LOADED;
    xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);
}

// void NvStorage::setPresets(const Messages::PresetsEventData &presetsData)
// {
//     assertNot0(presetsNvsHandle_, "presetsNvsHandle_");

//     ESP_ERROR_CHECK(nvs_set_u8(presetsNvsHandle_, "NumberOfPresets", presetsData.numberOfPresets));

//     for (uint8_t i = 0; i < presetsData.numberOfPresets; ++i)
//     {
//         const Messages::PresetEventData &preset = presetsData.presets[i];
//         char key[16];
//         snprintf(key, sizeof(key), "Preset%d", i);
//         ESP_ERROR_CHECK(nvs_set_blob(presetsNvsHandle_, key, &preset, sizeof(Messages::PresetEventData)));
//     }

//     ESP_ERROR_CHECK(nvs_commit(presetsNvsHandle_));
// }

// void NvStorage::requestPresets(Messages::PresetsEventData &presetsData)
// {
//     assertNot0(presetsNvsHandle_, "presetsNvsHandle_");

//     uint8_t number_of_presets;
//     ESP_ERROR_CHECK(nvs_get_u8(presetsNvsHandle_, "NumberOfPresets", &number_of_presets));
//     presetsData.numberOfPresets = number_of_presets;

//     for (uint8_t i = 0; i < number_of_presets; ++i)
//     {
//         char key[16];
//         snprintf(key, sizeof(key), "Preset%d", i);
//         Messages::PresetEventData &preset = presetsData.presets[i];
//         size_t length = sizeof(Messages::PresetEventData); // Length is not used in this case since
//                                                            // we expect a fixed size blob
//         ESP_ERROR_CHECK(nvs_get_blob(presetsNvsHandle_, key, &preset, &length));
//         presetsData.presets[i] = preset;
//     }

//     // Send configuration response message
//     Messages::Event responseEvent;
//     responseEvent.type = Messages::PRESETS_RESPONSE;
//     responseEvent.data.presetsData = presetsData;
//     xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);
// }

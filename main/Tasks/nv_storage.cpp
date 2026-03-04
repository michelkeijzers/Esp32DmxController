#include "nv_storage.hpp"
#include <cstring>
#include <esp_log.h>

NvStorage::NvStorage()
    : RtosTask(), configuration_nvs_handle(0), presets_nvs_handle(0), configuration_namespace_name("configuration"),
      presets_namespace_name("presets")
{
}

NvStorage::~NvStorage()
{
    if (configuration_nvs_handle != 0)
    {
        nvs_close(configuration_nvs_handle);
    }

    if (presets_nvs_handle != 0)
    {
        nvs_close(presets_nvs_handle);
    }
}

void NvStorage::init(RtosTask::TaskProperties taskProperties)
{
    RtosTask::init(taskProperties);
    ESP_ERROR_CHECK(nvs_open(configuration_namespace_name, NVS_READWRITE, &configuration_nvs_handle));
    ESP_ERROR_CHECK(nvs_open(presets_namespace_name, NVS_READWRITE, &presets_nvs_handle));
}

void NvStorage::taskEntry(void *param) { static_cast<NvStorage *>(param)->taskLoop(); }

void NvStorage::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(log_tag_, "NvStorage event received: %d", event.type);
            switch (event.type)
            {
            case Messages::SET_CONFIGURATION:
                setConfiguration(event.data.configurationData);
                break;

            case Messages::REQUEST_CONFIGURATION:
                requestConfiguration(event.data.configurationData);
                break;

            case Messages::SET_PRESETS:
                setPresets(event.data.presetsData);
                break;

            case Messages::REQUEST_PRESETS:
                requestPresets(event.data.presetsData);
                break;

            default:
                ESP_LOGW(log_tag_, "Unknown NvStorage event type: %d", event.type);
                break;
            }
        }
    }
}

void NvStorage::setConfiguration(const Messages::ConfigurationEventData &configurationData)
{
    assertNot0(configuration_nvs_handle, "configuration_nvs_handle");
    ESP_ERROR_CHECK(nvs_set_u8(
        configuration_nvs_handle, "SwitchPolarityNormallyOpen", configurationData.switchPolarityNormallyOpen));
    ESP_ERROR_CHECK(
        nvs_set_u16(configuration_nvs_handle, "LongPressThreshold", configurationData.longPressThresholdMs));
    ESP_ERROR_CHECK(nvs_commit(configuration_nvs_handle));
}

void NvStorage::requestConfiguration(Messages::ConfigurationEventData &configurationData)
{
    assertNot0(configuration_nvs_handle, "configuration_nvs_handle");

    uint8_t switch_polarity_normally_open;
    ESP_ERROR_CHECK(nvs_get_u8(configuration_nvs_handle, "SwitchPolarityNormallyOpen", &switch_polarity_normally_open));
    configurationData.switchPolarityNormallyOpen = switch_polarity_normally_open;

    uint16_t long_press_threshold_ms;
    ESP_ERROR_CHECK(nvs_get_u16(configuration_nvs_handle, "LongPressThreshold", &long_press_threshold_ms));
    configurationData.longPressThresholdMs = long_press_threshold_ms;

    // Send configuration response message
    Messages::Event responseEvent;
    responseEvent.type = Messages::CONFIGURATION_RESPONSE;
    responseEvent.data.configurationData = configurationData;
    xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);
}

void NvStorage::setPresets(const Messages::PresetsEventData &presetsData)
{
    assertNot0(presets_nvs_handle, "presets_nvs_handle");

    ESP_ERROR_CHECK(nvs_set_u8(presets_nvs_handle, "NumberOfPresets", presetsData.numberOfPresets));

    for (uint8_t i = 0; i < presetsData.numberOfPresets; ++i)
    {
        const Messages::PresetEventData &preset = presetsData.presets[i];
        char key[16];
        snprintf(key, sizeof(key), "Preset%d", i);
        ESP_ERROR_CHECK(nvs_set_blob(presets_nvs_handle, key, &preset, sizeof(Messages::PresetEventData)));
    }

    ESP_ERROR_CHECK(nvs_commit(presets_nvs_handle));
}

void NvStorage::requestPresets(Messages::PresetsEventData &presetsData)
{
    assertNot0(presets_nvs_handle, "presets_nvs_handle");

    uint8_t number_of_presets;
    ESP_ERROR_CHECK(nvs_get_u8(presets_nvs_handle, "NumberOfPresets", &number_of_presets));
    presetsData.numberOfPresets = number_of_presets;

    for (uint8_t i = 0; i < number_of_presets; ++i)
    {
        char key[16];
        snprintf(key, sizeof(key), "Preset%d", i);
        Messages::PresetEventData &preset = presetsData.presets[i];
        size_t length = sizeof(Messages::PresetEventData); // Length is not used in this case since
                                                           // we expect a fixed size blob
        ESP_ERROR_CHECK(nvs_get_blob(presets_nvs_handle, key, &preset, &length));
        presetsData.presets[i] = preset;
    }

    // Send configuration response message
    Messages::Event responseEvent;
    responseEvent.type = Messages::PRESETS_RESPONSE;
    responseEvent.data.presetsData = presetsData;
    xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);
}

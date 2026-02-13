#include "nvs_storage.hpp"
#include <cstring>
#include <esp_log.h>

static const char *LOG_TAG = "NvsStorage";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

NvsStorage::NvsStorage()
    : RtosTask(), configuration_nvs_handle(0), presets_nvs_handle(0),
      configuration_namespace_name("configuration"),
      presets_namespace_name("presets") {}

NvsStorage::~NvsStorage() {
    if (configuration_nvs_handle != 0) {
        nvs_close(configuration_nvs_handle);
    }

    if (presets_nvs_handle != 0) {
        nvs_close(presets_nvs_handle);
    }
}

esp_err_t NvsStorage::init(QueueHandle_t dmxControllerEventQueue) {
    if (RtosTask::init("NVSStorageTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY,
            sizeof(Messages::Event), dmxControllerEventQueue) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize NVSStorageTask");
        return ESP_FAIL;
    }

    esp_err_t err = nvs_open(
        configuration_namespace_name, NVS_READWRITE, &configuration_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to open configuration NVS namespace: %s",
            esp_err_to_name(err));
    }

    err = nvs_open(presets_namespace_name, NVS_READWRITE, &presets_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to open presets NVS namespace: %s",
            esp_err_to_name(err));
    }

    return err;
}

void NvsStorage::taskEntry(void *param) {
    static_cast<NvsStorage *>(param)->taskLoop();
}

void NvsStorage::taskLoop() {
    Messages::Event event;
    while (true) {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(LOG_TAG, "NVSStorage event received: %d", event.type);
            switch (event.type) {
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
                ESP_LOGW(
                    LOG_TAG, "Unknown NVSStorage event type: %d", event.type);
                break;
            }
        }
    }
}

esp_err_t NvsStorage::setConfiguration(
    const Messages::ConfigurationEventData &configurationData) {
    if (!configuration_nvs_handle)
        return ESP_ERR_INVALID_STATE;

    if (nvs_set_u8(configuration_nvs_handle, "SwitchPolarityInv",
            configurationData.switch_polarity_inverted) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to set switch polarity inverted");
        return ESP_FAIL;
    }

    if (nvs_set_u16(configuration_nvs_handle, "LongPressThreshold",
            configurationData.long_press_threshold_ms) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to set long press threshold");
        return ESP_FAIL;
    }

    if (nvs_commit(configuration_nvs_handle) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to commit configuration data");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t NvsStorage::requestConfiguration(
    Messages::ConfigurationEventData &configurationData) {
    if (!configuration_nvs_handle)
        return ESP_ERR_INVALID_STATE;

    uint8_t switch_polarity_inverted;
    if (nvs_get_u8(configuration_nvs_handle, "SwitchPolarityInv",
            &switch_polarity_inverted) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to get switch polarity inverted");
        return ESP_FAIL;
    }
    configurationData.switch_polarity_inverted = switch_polarity_inverted;

    uint16_t long_press_threshold_ms;
    if (nvs_get_u16(configuration_nvs_handle, "LongPressThreshold",
            &long_press_threshold_ms) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to get long press threshold");
        return ESP_FAIL;
    }
    configurationData.long_press_threshold_ms = long_press_threshold_ms;

    // Send configuration response message
    Messages::Event responseEvent;
    responseEvent.type = Messages::CONFIGURATION_RESPONSE;
    responseEvent.data.configurationData = configurationData;
    xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);

    return ESP_OK;
}

esp_err_t NvsStorage::setPresets(
    const Messages::PresetsEventData &presetsData) {
    if (!presets_nvs_handle)
        return ESP_ERR_INVALID_STATE;

    if (nvs_set_u8(presets_nvs_handle, "NumberOfPresets",
            presetsData.number_of_presets) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to set number of presets");
        return ESP_FAIL;
    }

    for (uint8_t i = 0; i < presetsData.number_of_presets; ++i) {
        const Messages::PresetEventData &preset = presetsData.presets[i];
        char key[16];
        snprintf(key, sizeof(key), "Preset%d", i);
        if (nvs_set_blob(presets_nvs_handle, key, &preset,
                sizeof(Messages::PresetEventData)) != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Failed to set preset %d", i);
            return ESP_FAIL;
        }
    }

    if (nvs_commit(presets_nvs_handle) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to commit presets data");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t NvsStorage::requestPresets(Messages::PresetsEventData &presetsData) {
    if (!presets_nvs_handle)
        return ESP_ERR_INVALID_STATE;

    uint8_t number_of_presets;
    if (nvs_get_u8(presets_nvs_handle, "NumberOfPresets", &number_of_presets) !=
        ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to get number of presets");
        return ESP_FAIL;
    }

    presetsData.number_of_presets = number_of_presets;

    for (uint8_t i = 0; i < number_of_presets; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "Preset%d", i);
        Messages::PresetEventData &preset = presetsData.presets[i];
        size_t length = sizeof(
            Messages::PresetEventData); // Length is not used in this case since
                                        // we expect a fixed size blob
        esp_err_t err = nvs_get_blob(presets_nvs_handle, key, &preset, &length);
        if (err != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Failed to get preset %d", i);
            return err;
        }
        presetsData.presets[i] = preset;
    }

    return ESP_OK;
}

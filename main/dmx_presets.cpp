#include "dmx_presets.hpp"
#include <esp_log.h>
#include <cstring>

static const char *TAG = "DmxPresets";

DmxPresets::DmxPresets()
    : numPresets_(20) // Default to 20 presets
{
    presets_.resize(MAX_PRESETS);
}

esp_err_t DmxPresets::init()
{
    ESP_LOGI(TAG, "DmxPresets initialized with %d presets", numPresets_);
    return ESP_OK;
}

esp_err_t DmxPresets::setNumPresets(uint8_t numPresets)
{
    if (numPresets < MIN_PRESETS || numPresets > MAX_PRESETS)
    {
        ESP_LOGE(TAG, "Invalid number of presets: %d (must be %d-%d)",
                 numPresets, MIN_PRESETS, MAX_PRESETS);
        return ESP_ERR_INVALID_ARG;
    }

    numPresets_ = numPresets;

    // Ensure current preset index is valid
    if (currentPresetIndex_ >= numPresets_)
    {
        currentPresetIndex_ = 0;
    }

    return ESP_OK; // TODO
}

DmxPreset *DmxPresets::getPreset(uint8_t index)
{
    if (index >= numPresets_)
    {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return nullptr;
    }
    return &presets_[index];
}

const DmxPreset *DmxPresets::getPreset(uint8_t index) const
{
    if (index >= numPresets_)
    {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return nullptr;
    }
    return &presets_[index];
}

esp_err_t DmxPresets::setPreset(uint8_t index, const DmxPreset &preset)
{
    if (index >= numPresets_)
    {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return ESP_ERR_INVALID_ARG;
    }

    // TODO presets_[index].copyFrom(preset);
    return ESP_OK; // TODO
}

esp_err_t DmxPresets::saveToNVRAM()
{
    // TODO if (!nvs_)
    if (true)
    {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret;

    // Save number of presets
    ret = ESP_OK; // nvs_->storeInt(NVS_KEY_NUM_PRESETS, numPresets_);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save num_presets to NVRAM");
        return ret;
    }

    // Save current preset index
    ret = ESP_OK; // TODO  nvs_->storeInt(NVS_KEY_CURRENT_PRESET, currentPresetIndex_);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save current_preset to NVRAM");
        return ret;
    }

    // Save each preset
    for (uint8_t i = 0; i < numPresets_; i++)
    {
        char key[32];
        // TODO snprintf(key, sizeof(key), "%s%d", NVS_KEY_PRESET_PREFIX, i);

        // Calculate total size: name (32) + universe1 (512) + universe2 (512) = 1056 bytes
        uint8_t buffer[32 + DMX_UNIVERSE_SIZE * 2];
        size_t offset = 0;

        // Copy name
        memcpy(buffer + offset, presets_[i].getName(), 32);
        offset += 32;

        // Copy universe 1
        memcpy(buffer + offset, presets_[i].getUniverseData(0), DMX_UNIVERSE_SIZE);
        offset += DMX_UNIVERSE_SIZE;

        // Copy universe 2
        memcpy(buffer + offset, presets_[i].getUniverseData(1), DMX_UNIVERSE_SIZE);
        offset += DMX_UNIVERSE_SIZE;

        // TODO ret = nvs_->storeBlob(key, buffer, sizeof(buffer));
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to save preset %d to NVRAM", i);
            return ret;
        }
    }

    ESP_LOGI(TAG, "Saved %d presets to NVRAM", numPresets_);
    return ESP_OK;
}

esp_err_t DmxPresets::loadFromNVRAM()
{
    if (true) // TODO if (!nvs_)
    {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret;

    // Load number of presets
    int32_t numPresets;
    ret = ESP_OK; // TODO nvs_->getInt(NVS_KEY_NUM_PRESETS, &numPresets);
    if (ret == ESP_OK && numPresets >= MIN_PRESETS && numPresets <= MAX_PRESETS)
    {
        numPresets_ = (uint8_t)numPresets;
    }
    else
    {
        ESP_LOGW(TAG, "Invalid or missing num_presets in NVRAM, using default");
        numPresets_ = 10;
    }

    // Load current preset index
    int32_t currentPreset;
    ret = ESP_OK; // TODO  = nvs_->getInt(NVS_KEY_CURRENT_PRESET, &currentPreset);
    if (ret == ESP_OK && currentPreset >= 0 && currentPreset < numPresets_)
    {
        currentPresetIndex_ = (uint8_t)currentPreset;
    }
    else
    {
        ESP_LOGW(TAG, "Invalid or missing current_preset in NVRAM, using 0");
        currentPresetIndex_ = 0;
    }

    // Load each preset
    for (uint8_t i = 0; i < numPresets_; i++)
    {
        char key[32];
        // TODO snprintf(key, sizeof(key), "%s%d", NVS_KEY_PRESET_PREFIX, i);

        uint8_t buffer[32 + DMX_UNIVERSE_SIZE * 2];
        size_t bufferSize = sizeof(buffer);
        size_t actualLength = 0;

        ret = ESP_OK; // TODO nvs_->getBlob(key, buffer, bufferSize, &actualLength);
        if (ret == ESP_OK && actualLength == sizeof(buffer))
        {
            size_t offset = 0;

            // Load name
            char name[32];
            memcpy(name, buffer + offset, 32);
            name[31] = '\0'; // Ensure null termination
            presets_[i].setName(name);
            offset += 32;

            // Load universe 1
            presets_[i].setUniverseData(0, buffer + offset, DMX_UNIVERSE_SIZE);
            offset += DMX_UNIVERSE_SIZE;

            // Load universe 2
            presets_[i].setUniverseData(1, buffer + offset, DMX_UNIVERSE_SIZE);
            offset += DMX_UNIVERSE_SIZE;
        }
        else
        {
            ESP_LOGW(TAG, "Failed to load preset %d from NVRAM, using empty preset", i);
            presets_[i].clear();
        }
    }

    ESP_LOGI(TAG, "Loaded %d presets from NVRAM", numPresets_);
    return ESP_OK;
}

void DmxPresets::clearAll()
{
    for (auto &preset : presets_)
    {
        preset.clear();
    }
    currentPresetIndex_ = 0;
}

void DmxPresets::setCurrentPresetIndex(uint8_t index)
{
    if (index < numPresets_)
    {
        currentPresetIndex_ = index;
        // Note: We don't save to NVRAM here for performance, it will be saved when presets change
    }
    else
    {
        ESP_LOGE(TAG, "Invalid preset index %d (max %d)", index, numPresets_ - 1);
    }
}

uint8_t DmxPresets::nextPreset()
{
    currentPresetIndex_ = (currentPresetIndex_ + 1) % numPresets_;
    return currentPresetIndex_;
}

uint8_t DmxPresets::previousPreset()
{
    currentPresetIndex_ = (currentPresetIndex_ - 1 + numPresets_) % numPresets_;
    return currentPresetIndex_;
}
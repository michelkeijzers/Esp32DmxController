#include "dmx_presets.hpp"
#include <cstring>

#include <esp_log.h>
#include <stdexcept>

static const char *LOG_TAG = "DmxPresets";

DmxPresets::DmxPresets() : numberOfFilledPresets_(20) // Default to 20 presets
{
    presets_.resize(MAX_PRESETS);
}

esp_err_t DmxPresets::init()
{
    ESP_LOGI(LOG_TAG, "DmxPresets initialized with %d filled presets", numberOfFilledPresets_);
    return ESP_OK;
}

esp_err_t DmxPresets::setNumberOfFilledPresets(uint8_t numberOfFilledPresets)
{
    if (numberOfFilledPresets < MIN_PRESETS || numberOfFilledPresets > MAX_PRESETS)
    {
        ESP_LOGE(LOG_TAG, "Invalid number of filled presets: %d (must be %d-%d)", numberOfFilledPresets, MIN_PRESETS,
            MAX_PRESETS);
        return ESP_ERR_INVALID_ARG;
    }

    numberOfFilledPresets_ = numberOfFilledPresets;

    // Ensure current preset index is valid
    if (currentPresetIndex_ >= numberOfFilledPresets_)
    {
        currentPresetIndex_ = 0;
    }

    return ESP_OK; // TODO
}

esp_err_t DmxPresets::addPreset(uint8_t presetNumber, const char *name, const uint8_t *dmxValues)
{
    if (presetNumber >= numberOfFilledPresets_)
    {
        ESP_LOGE(LOG_TAG, "Preset index %d out of range (max %d)", presetNumber, numberOfFilledPresets_ - 1);
        return ESP_ERR_INVALID_ARG;
    }

    if (!name || !dmxValues)
    {
        ESP_LOGE(LOG_TAG, "Invalid pointer arguments");
        return ESP_ERR_INVALID_ARG;
    }

    presets_[presetNumber].setIndex(presetNumber);
    presets_[presetNumber].setName(name);
    presets_[presetNumber].setDmxValues(dmxValues);

    ESP_LOGI(LOG_TAG, "Added preset at index %d: %s", presetNumber, name);
    return ESP_OK;
}

DmxPreset &DmxPresets::getPreset(uint8_t index)
{
    if (index >= numberOfFilledPresets_ || !presets_[index].isInitialized())
    {
        ESP_LOGE(LOG_TAG, "Preset index %d is invalid or uninitialized (max %d)", index, numberOfFilledPresets_ - 1);
        // Return first preset as fallback (or handle as needed)
        return presets_[0];
    }
    return presets_[index];
}

esp_err_t DmxPresets::setPreset(uint8_t index, const DmxPreset &preset)
{
    if (index >= numberOfFilledPresets_)
    {
        ESP_LOGE(LOG_TAG, "Preset index %d out of range (max %d)", index, numberOfFilledPresets_ - 1);
        return ESP_ERR_INVALID_ARG;
    }

    presets_[index].copyFrom(preset);
    return ESP_OK;
}

void DmxPresets::clearAll()
{
    for (auto &preset : presets_)
    {
        preset.clear();
    }
    currentPresetIndex_ = 0;
    // numberOfFilledPresets_ remains unchanged to preserve test setup consistency
}

void DmxPresets::setCurrentPresetIndex(uint8_t index)
{
    if (index < numberOfFilledPresets_)
    {
        currentPresetIndex_ = index;
        // Note: We don't save to NVRAM here for performance, it will be saved when presets change
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Invalid preset index %d (max %d)", index, numberOfFilledPresets_ - 1);
    }
}

uint8_t DmxPresets::selectNextPreset()
{
    currentPresetIndex_ = (currentPresetIndex_ + 1) % numberOfFilledPresets_;
    return currentPresetIndex_;
}

uint8_t DmxPresets::selectPreviousPreset()
{
    currentPresetIndex_ = (currentPresetIndex_ - 1 + numberOfFilledPresets_) % numberOfFilledPresets_;
    return currentPresetIndex_;
}


#include "dmx_presets.hpp"
#include <cstring>
#include <esp_log.h>

static const char *TAG = "DmxPresets";

DmxPresets::DmxPresets()
    : numPresets_(20) // Default to 20 presets
{
    presets_.resize(MAX_PRESETS);
}

esp_err_t DmxPresets::init() {
    ESP_LOGI(TAG, "DmxPresets initialized with %d presets", numPresets_);
    return ESP_OK;
}

esp_err_t DmxPresets::setNumPresets(uint8_t numPresets) {
    if (numPresets < MIN_PRESETS || numPresets > MAX_PRESETS) {
        ESP_LOGE(TAG, "Invalid number of presets: %d (must be %d-%d)", numPresets, MIN_PRESETS, MAX_PRESETS);
        return ESP_ERR_INVALID_ARG;
    }

    numPresets_ = numPresets;

    // Ensure current preset index is valid
    if (currentPresetIndex_ >= numPresets_) {
        currentPresetIndex_ = 0;
    }

    return ESP_OK; // TODO
}

esp_err_t DmxPresets::addPreset(const char *name, uint16_t universe_1_length, const uint8_t *universe_1_data,
    uint16_t universe_2_length, const uint8_t *universe_2_data) {
    uint8_t index = presets_.size();
    if (index >= numPresets_) {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return ESP_ERR_INVALID_ARG;
    }

    if (!name || !universe_1_data || !universe_2_data) {
        ESP_LOGE(TAG, "Invalid pointer arguments");
        return ESP_ERR_INVALID_ARG;
    }

    presets_[index].setName(name);
    presets_[index].setUniverseData(0, universe_1_data, universe_1_length);
    presets_[index].setUniverseData(1, universe_2_data, universe_2_length);

    ESP_LOGI(TAG, "Added preset at index %d: %s", index, name);
    return ESP_OK;
}

DmxPreset *DmxPresets::getPreset(uint8_t index) {
    if (index >= numPresets_) {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return nullptr;
    }
    return &presets_[index];
}

const DmxPreset *DmxPresets::getPreset(uint8_t index) const {
    if (index >= numPresets_) {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return nullptr;
    }
    return &presets_[index];
}

esp_err_t DmxPresets::setPreset(uint8_t index, const DmxPreset &preset) {
    if (index >= numPresets_) {
        ESP_LOGE(TAG, "Preset index %d out of range (max %d)", index, numPresets_ - 1);
        return ESP_ERR_INVALID_ARG;
    }

    // TODO presets_[index].copyFrom(preset);
    return ESP_OK; // TODO
}

void DmxPresets::clearAll() {
    for (auto &preset : presets_) {
        preset.clear();
    }
    currentPresetIndex_ = 0;
}

void DmxPresets::setCurrentPresetIndex(uint8_t index) {
    if (index < numPresets_) {
        currentPresetIndex_ = index;
        // Note: We don't save to NVRAM here for performance, it will be saved when presets change
    } else {
        ESP_LOGE(TAG, "Invalid preset index %d (max %d)", index, numPresets_ - 1);
    }
}

uint8_t DmxPresets::nextPreset() {
    currentPresetIndex_ = (currentPresetIndex_ + 1) % numPresets_;
    return currentPresetIndex_;
}

uint8_t DmxPresets::previousPreset() {
    currentPresetIndex_ = (currentPresetIndex_ - 1 + numPresets_) % numPresets_;
    return currentPresetIndex_;
}
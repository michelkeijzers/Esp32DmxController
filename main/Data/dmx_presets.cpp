#include "dmx_presets.hpp"
#include "../Base/assert.hpp"
#include <cstring>
#include <esp_log.h>
#include <stdexcept>

static const char *LOG_TAG = "DmxPresets";

DmxPresets::DmxPresets(IAssert *assert) : assert_(assert)
{
    presets_.clear();
    for (int i = 0; i < MAX_PRESETS; ++i)
    {
        presets_.push_back(DmxPreset(assert_));
    }
    numberOfFilledPresets_ = 0;
    currentPresetIndex_ = 0;
}

void DmxPresets::setNumberOfFilledPresets(uint8_t numberOfFilledPresets)
{
    if (numberOfFilledPresets < MIN_PRESETS || numberOfFilledPresets > MAX_PRESETS)
    {
        ESP_LOGE(LOG_TAG, "Invalid number of filled presets: %d (must be %d-%d)", numberOfFilledPresets, MIN_PRESETS,
            MAX_PRESETS);
        // TODO: call software error. Put in a better place (cannot call rtostask).
        // char msg[80];
        // snprintf(msg, sizeof(msg), "Preset '%s' error: numberOfFilledPresets = %d", presetName,
        // numberOfFilledPresets); softwareError(msg);
    }

    numberOfFilledPresets_ = numberOfFilledPresets;

    // Ensure current preset index is valid
    if (currentPresetIndex_ >= numberOfFilledPresets_)
    {
        currentPresetIndex_ = 0;
    }
}

void DmxPresets::addPreset(uint8_t presetNumber, const char *name, const uint8_t *dmxValues)
{
    assert_->assertNotNull(name, "name");
    assert_->assertNotNull(dmxValues, "dmxValues");

    if (presetNumber >= numberOfFilledPresets_)
    {
        ESP_LOGE(LOG_TAG, "Preset index %d out of range (max %d)", presetNumber, numberOfFilledPresets_ - 1);
        // TODO: call software error. Put in a better place (cannot call rtostask).
        // char msg[80];
        // snprintf(msg, sizeof(msg), "Preset '%s' error: numberOfFilledPresets = %d", presetName,
        // numberOfFilledPresets); softwareError(msg);
    }

    // TODO: Call assertNotNull
    if (!name || !dmxValues)
    {
        ESP_LOGE(LOG_TAG, "Invalid pointer arguments");
    }

    presets_[presetNumber].setIndex(presetNumber);
    presets_[presetNumber].setName(name);
    presets_[presetNumber].setDmxValues(dmxValues);

    ESP_LOGI(LOG_TAG, "Added preset at index %d: %s", presetNumber, name);
    numberOfFilledPresets_++; // TODO: Check if ok (what if overwritten by nvram and this is called, double increment?)
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

void DmxPresets::setPreset(uint8_t index, const DmxPreset &preset)
{
    if (index >= numberOfFilledPresets_)
    {
        ESP_LOGE(LOG_TAG, "Preset index %d out of range (max %d)", index, numberOfFilledPresets_ - 1);
        // TODO: call software error. Put in a better place (cannot call rtostask).
    }

    presets_[index].copyFrom(preset);
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

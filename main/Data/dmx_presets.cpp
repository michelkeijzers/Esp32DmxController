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
    assert_->assertTrue(numberOfFilledPresets >= MIN_PRESETS && numberOfFilledPresets <= MAX_PRESETS,
        "numberOfFilledPresets out of range");

    numberOfFilledPresets_ = numberOfFilledPresets;

    // Ensure current preset index is valid
    if (currentPresetIndex_ >= numberOfFilledPresets_)
    {
        currentPresetIndex_ = numberOfFilledPresets_ - 1;
    }
}

void DmxPresets::addPreset(uint8_t presetNumber, const char *name, const uint8_t *dmxValues)
{
    assert_->assertTrue(presetNumber < MAX_PRESETS, "Preset number out of range");
    assert_->assertNotNull(name, "name");
    assert_->assertNotNull(dmxValues, "dmxValues");

    presets_[presetNumber].setIndex(presetNumber);
    presets_[presetNumber].setName(name);
    presets_[presetNumber].setDmxValues(dmxValues);

    ESP_LOGI(LOG_TAG, "Added preset at index %d: %s", presetNumber, name);
    numberOfFilledPresets_++; // TODO: Check if ok (what if overwritten by nvram and this is called, double increment?)
}

DmxPreset &DmxPresets::getPreset(uint8_t index)
{
    assert_->assertTrue(index < numberOfFilledPresets_, "Preset index out of range");
    assert_->assertTrue(presets_[index].isInitialized(), "Preset at index is not initialized");

    return presets_[index];
}

void DmxPresets::setPreset(uint8_t index, const DmxPreset &preset)
{
    assert_->assertTrue(index < numberOfFilledPresets_, "Preset index out of range");
    assert_->assertTrue(presets_[index].isInitialized(), "Preset at index is not initialized");

    presets_[index].copyFrom(preset);
}

void DmxPresets::setCurrentPresetIndex(uint8_t index)
{
    assert_->assertTrue(index < numberOfFilledPresets_, "Preset index out of range");
    assert_->assertTrue(presets_[index].isInitialized(), "Preset at index is not initialized");

    currentPresetIndex_ = index;
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

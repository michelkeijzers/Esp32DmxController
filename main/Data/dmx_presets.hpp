#pragma once

#include "dmx_preset.hpp"
#include <esp_err.h>
#include <vector>
// Maximum number of presets
#define MAX_PRESETS 20
#define MIN_PRESETS 2

class DmxPresets
{
  public:
    DmxPresets();

    esp_err_t init();

    uint8_t getNumberOfFilledPresets() const { return numberOfFilledPresets_; }
    esp_err_t setNumberOfFilledPresets(uint8_t numberOfFilledPresets);

    DmxPreset &getCurrentPreset() { return presets_[currentPresetIndex_]; }
    DmxPreset &getPreset(uint8_t index);
    esp_err_t setPreset(uint8_t index, const DmxPreset &preset);
    esp_err_t addPreset(uint8_t presetNumber, const char *name, const uint8_t *dmxValues);
    void clearAll();

    uint8_t getCurrentPresetIndex() const { return currentPresetIndex_; }
    void setCurrentPresetIndex(uint8_t index);
    uint8_t selectNextPreset();
    uint8_t selectPreviousPreset();

  private:
    uint8_t numberOfFilledPresets_;
    uint8_t currentPresetIndex_;
    std::vector<DmxPreset> presets_;
};
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
    // Constructor
    DmxPresets();

    // Initialize with NVS storage
    esp_err_t init();

    // Set number of presets (2-20)
    esp_err_t setNumPresets(uint8_t numPresets);
    esp_err_t addPreset(uint8_t presetNumber, const char *name, uint16_t universe11Length,
        const uint8_t *universe1Ddata, uint16_t universe2Length, const uint8_t *universe2Data);

    // Get number of presets
    uint8_t getNumPresets() const { return numPresets_; }

    // Get preset by index
    DmxPreset &getPreset(uint8_t index);
    DmxPreset &getCurrentPreset() { return presets_[currentPresetIndex_]; }

    // Set preset data
    esp_err_t setPreset(uint8_t index, const DmxPreset &preset);

    // Clear all presets
    void clearAll();

    // Get current preset index
    uint8_t getCurrentPresetIndex() const { return currentPresetIndex_; }

    // Set current preset index
    void setCurrentPresetIndex(uint8_t index);

    // Move to next preset (with wraparound)
    uint8_t selectNextPreset();

    // Move to previous preset (with wraparound)
    uint8_t selectPreviousPreset();

  private:
    uint8_t numPresets_;
    uint8_t currentPresetIndex_;
    std::vector<DmxPreset> presets_;
};
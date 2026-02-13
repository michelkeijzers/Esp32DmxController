#pragma once

#include "dmx_preset.hpp"
#include <esp_err.h>
#include <vector>
// Maximum number of presets
#define MAX_PRESETS 20
#define MIN_PRESETS 2

class DmxPresets {
  public:
    // Constructor
    DmxPresets();

    // Initialize with NVS storage
    esp_err_t init();

    // Set number of presets (2-20)
    esp_err_t setNumPresets(uint8_t numPresets);
    esp_err_t addPreset(const char *name, uint16_t universe_1_length, const uint8_t *universe_1_data,
        uint16_t universe_2_length, const uint8_t *universe_2_data);

    // Get number of presets
    uint8_t getNumPresets() const { return numPresets_; }

    // Get preset by index
    DmxPreset *getPreset(uint8_t index);
    const DmxPreset *getPreset(uint8_t index) const;

    // Set preset data
    esp_err_t setPreset(uint8_t index, const DmxPreset &preset);

    // Clear all presets
    void clearAll();

    // Get current preset index
    uint8_t getCurrentPresetIndex() const { return currentPresetIndex_; }

    // Set current preset index
    void setCurrentPresetIndex(uint8_t index);

    // Move to next preset (with wraparound)
    uint8_t nextPreset();

    // Move to previous preset (with wraparound)
    uint8_t previousPreset();

  private:
    uint8_t numPresets_;
    uint8_t currentPresetIndex_;
    std::vector<DmxPreset> presets_;


};
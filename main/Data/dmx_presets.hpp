#pragma once

#include "../Base/lockable.hpp"
#include "dmx_preset.hpp"
#include <esp_err.h>
#include <vector>

#define MAX_PRESETS 20
#define MIN_PRESETS 2

class DmxPresets : public Lockable
{
  public:
    DmxPresets();

    uint8_t getNumberOfFilledPresets() const { return numberOfFilledPresets_; }
    void setNumberOfFilledPresets(uint8_t numberOfFilledPresets);

    DmxPreset &getCurrentPreset() { return presets_[currentPresetIndex_]; }
    DmxPreset &getPreset(uint8_t index);
    void setPreset(uint8_t index, const DmxPreset &preset);
    void addPreset(uint8_t presetNumber, const char *name, const uint8_t *dmxValues);
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
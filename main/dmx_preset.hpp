#pragma once

#include "dmx_preset.hpp"
#include <cstring>
#include <stdint.h>
#include <string>
// DMX Universe size
const uint16_t DMX_UNIVERSE_SIZE = 512;

class DmxPreset
{
  public:
    // Constructor
    DmxPreset();

    // Set preset name
    void setIndex(uint16_t index) { index_ = index; }
    uint16_t getIndex() const { return index_; }

    void setName(const char *name);
    const char *getName() const;

    // Set DMX values for a universe
    void setUniverseValue(uint8_t universe, uint16_t channel, uint8_t value);
    uint8_t getUniverseValue(uint8_t universe, uint16_t channel) const;

    // Set entire universe data
    void setUniverseData(uint8_t universe, const uint8_t *data, size_t length);
    const uint8_t *getUniverseData(uint8_t universe) const;
    uint16_t getUniverseLength(uint8_t universe) const;

    // Clear/reset preset
    void clear();

    // Copy from another preset
    void copyFrom(const DmxPreset &other);

  private:
    uint8_t index_;
    char name_[32];                        // Preset name (max 31 chars + null)
    uint8_t universe1_[DMX_UNIVERSE_SIZE]; // Universe 1 data (512 channels)
    uint16_t universe1Length_;
    uint8_t universe2_[DMX_UNIVERSE_SIZE]; // Universe 2 data (512 channels)
    uint16_t universe2Length_;
};
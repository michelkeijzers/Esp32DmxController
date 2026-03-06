#pragma once

#include "dmx_preset.hpp"
#include <cstring>
#include <stdint.h>
#include <string>

const uint16_t NR_OF_DMX_CHANNELS = 512;

class IAssert;
class DmxPreset
{
  public:
    // Constructor
    DmxPreset(IAssert *assert);

    // Set preset name
    void setIndex(uint16_t index) { index_ = static_cast<uint8_t>(index); }
    uint16_t getIndex() const { return index_; }

    void setName(const char *name);
    const char *getName() const;

    bool isInitialized() const { return name_[0] != '\0'; }

    uint8_t getDmxValue(uint16_t channel) const;
    void setDmxValue(uint16_t channel, uint8_t value);
    const uint8_t *getDmxValues() const { return dmxValues_; }
    void setDmxValues(const uint8_t *values);

    // Clear/reset preset
    void clear();

    // Copy from another preset
    void copyFrom(const DmxPreset &other);

  private:
    uint16_t index_;
    char name_[32];
    uint8_t dmxValues_[NR_OF_DMX_CHANNELS];
    IAssert *assert_;
};
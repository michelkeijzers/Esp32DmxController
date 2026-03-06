#include "../Base/assert.hpp"

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include "dmx_preset.hpp"
#include <esp_log.h>

DmxPreset::DmxPreset(IAssert *assert) : assert_(assert) { clear(); }

void DmxPreset::setName(const char *name)
{
    assert_->assertNotNull(name, "Preset name is null");
    strncpy(name_, name, sizeof(name_) - 1);
    name_[sizeof(name_) - 1] = '\0'; // Ensure null termination
}

const char *DmxPreset::getName() const { return name_; }

void DmxPreset::setDmxValue(uint16_t channel, uint8_t value)
{
    assert_->assertTrue(channel < NR_OF_DMX_CHANNELS, "Channel index out of range");
    dmxValues_[channel] = value;
}

uint8_t DmxPreset::getDmxValue(uint16_t channel) const
{
    assert_->assertTrue(channel < NR_OF_DMX_CHANNELS, "Channel index out of range");
    return dmxValues_[channel];
}

void DmxPreset::setDmxValues(const uint8_t *values)
{
    assert_->assertNotNull(values, "DMX values pointer is null");
    memcpy(dmxValues_, values, NR_OF_DMX_CHANNELS);
}

void DmxPreset::clear()
{
    index_ = 0;
    memset(name_, 0, sizeof(name_));
    memset(dmxValues_, 0, NR_OF_DMX_CHANNELS);
}

void DmxPreset::copyFrom(const DmxPreset &other)
{
    index_ = other.getIndex();
    memcpy(name_, other.getName(), sizeof(name_)); // TODO: +1 for \0 ?
    memcpy(dmxValues_, other.getDmxValues(), NR_OF_DMX_CHANNELS);
}
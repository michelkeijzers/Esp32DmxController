#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include "dmx_preset.hpp"
#include <esp_log.h>

static const char *TAG = "DmxPreset";

DmxPreset::DmxPreset() { clear(); }

void DmxPreset::setName(const char *name)
{
    if (name)
    {
        strncpy(name_, name, sizeof(name_) - 1);
        name_[sizeof(name_) - 1] = '\0'; // Ensure null termination
        name_[sizeof(name_) - 1] = '\0'; // Ensure null termination
    }
    else
    {
        name_[0] = '\0';
    }
}

const char *DmxPreset::getName() const { return name_; }

void DmxPreset::setDmxValue(uint16_t channel, uint8_t value)
{
    if (channel >= NR_OF_DMX_CHANNELS)
    {
        ESP_LOGE(TAG, "Channel %d out of range (max %d)", channel, NR_OF_DMX_CHANNELS - 1);
        return;
    }

    dmxValues_[channel] = value;
}

uint8_t DmxPreset::getDmxValue(uint16_t channel) const
{
    if (channel >= NR_OF_DMX_CHANNELS)
    {
        ESP_LOGE(TAG, "Channel %d out of range (max %d)", channel, NR_OF_DMX_CHANNELS - 1);
        return 0;
    }

    return dmxValues_[channel];
}

void DmxPreset::setDmxValues(const uint8_t *values)
{
    if (!values)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return;
    }

    memcpy(dmxValues_, values, NR_OF_DMX_CHANNELS);
}

void DmxPreset::clear()
{
    memset(name_, 0, sizeof(name_));
    memset(dmxValues_, 0, NR_OF_DMX_CHANNELS);
}

void DmxPreset::copyFrom(const DmxPreset &other)
{
    index_ = other.getIndex();
    memcpy(name_, other.getName(), sizeof(name_)); // TODO: +1 for \0 ?
    memcpy(dmxValues_, other.getDmxValues(), NR_OF_DMX_CHANNELS);
}
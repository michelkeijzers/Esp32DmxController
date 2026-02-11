#include "dmx_preset.hpp"
#include <esp_log.h>

static const char *TAG = "DmxPreset";

DmxPreset::DmxPreset()
{
    clear();
}

void DmxPreset::setName(const char *name)
{
    if (name)
    {
        strncpy(name_, name, sizeof(name_) - 1);
        name_[sizeof(name_) - 1] = '\0'; // Ensure null termination
    }
    else
    {
        name_[0] = '\0';
    }
}

const char *DmxPreset::getName() const
{
    return name_;
}

void DmxPreset::setUniverseValue(uint8_t universe, uint16_t channel, uint8_t value)
{
    if (channel >= DMX_UNIVERSE_SIZE)
    {
        ESP_LOGE(TAG, "Channel %d out of range (max %d)", channel, DMX_UNIVERSE_SIZE - 1);
        return;
    }

    if (universe == 0)
    {
        universe1_[channel] = value;
    }
    else if (universe == 1)
    {
        universe2_[channel] = value;
    }
    else
    {
        ESP_LOGE(TAG, "Universe %d out of range (max 1)", universe);
    }
}

uint8_t DmxPreset::getUniverseValue(uint8_t universe, uint16_t channel) const
{
    if (channel >= DMX_UNIVERSE_SIZE)
    {
        ESP_LOGE(TAG, "Channel %d out of range (max %d)", channel, DMX_UNIVERSE_SIZE - 1);
        return 0;
    }

    if (universe == 0)
    {
        return universe1_[channel];
    }
    else if (universe == 1)
    {
        return universe2_[channel];
    }
    else
    {
        ESP_LOGE(TAG, "Universe %d out of range (max 1)", universe);
        return 0;
    }
}

void DmxPreset::setUniverseData(uint8_t universe, const uint8_t *data, size_t length)
{
    if (!data)
    {
        ESP_LOGE(TAG, "Invalid data pointer");
        return;
    }

    size_t copyLength = (length > DMX_UNIVERSE_SIZE) ? DMX_UNIVERSE_SIZE : length;

    if (universe == 0)
    {
        memcpy(universe1_, data, copyLength);
        if (copyLength < DMX_UNIVERSE_SIZE)
        {
            memset(universe1_ + copyLength, 0, DMX_UNIVERSE_SIZE - copyLength);
        }
    }
    else if (universe == 1)
    {
        memcpy(universe2_, data, copyLength);
        if (copyLength < DMX_UNIVERSE_SIZE)
        {
            memset(universe2_ + copyLength, 0, DMX_UNIVERSE_SIZE - copyLength);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Universe %d out of range (max 1)", universe);
    }
}

const uint8_t *DmxPreset::getUniverseData(uint8_t universe) const
{
    if (universe == 0)
    {
        return universe1_;
    }
    else if (universe == 1)
    {
        return universe2_;
    }
    else
    {
        ESP_LOGE(TAG, "Universe %d out of range (max 1)", universe);
        return nullptr;
    }
}

void DmxPreset::clear()
{
    memset(name_, 0, sizeof(name_));
    memset(universe1_, 0, sizeof(universe1_));
    memset(universe2_, 0, sizeof(universe2_));
}

void DmxPreset::copyFrom(const DmxPreset &other)
{
    strcpy(name_, other.name_);
    memcpy(universe1_, other.universe1_, sizeof(universe1_));
    memcpy(universe2_, other.universe2_, sizeof(universe2_));
}
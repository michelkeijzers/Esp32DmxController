#include "configuration.hpp"
#include "../Base/assert.hpp"

Configuration::Configuration(IAssert *assert)
    : footSwitchPolarityNormallyOpen_(false), footSwitchLongPressTime_(0), circularPresetNavigation_(false),
      numberOfFilledPresets_(0), assert_(assert)
{
}

bool Configuration::getFootSwitchPolarityNormallyOpen() const { return footSwitchPolarityNormallyOpen_; }

void Configuration::setFootSwitchPolarityNormallyOpen(bool normallyOpen)
{
    footSwitchPolarityNormallyOpen_ = normallyOpen;
}

uint16_t Configuration::getFootSwitchLongPressTime() const { return footSwitchLongPressTime_; }

void Configuration::setFootSwitchLongPressTime(uint16_t longPressTime) { footSwitchLongPressTime_ = longPressTime; }

bool Configuration::getCircularPresetNavigation() const { return circularPresetNavigation_; }

void Configuration::setCircularPresetNavigation(bool circular) { circularPresetNavigation_ = circular; }

uint8_t Configuration::getNumberOfFilledPresets() const { return numberOfFilledPresets_; }

void Configuration::setNumberOfFilledPresets(uint8_t numberOfFilledPresets)
{
    numberOfFilledPresets_ = numberOfFilledPresets;
}

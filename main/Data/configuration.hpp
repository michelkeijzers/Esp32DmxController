#pragma once
#include "../Base/lockable.hpp"
#include <cstdint>

class Configuration : public Lockable
{
  public:
    Configuration();

    bool getFootSwitchPolarityNormallyOpen() const;
    void setFootSwitchPolarityNormallyOpen(bool normallyOpen);

    uint16_t getFootSwitchLongPressTime() const;
    void setFootSwitchLongPressTime(uint16_t longPressTime);

    bool getCircularPresetNavigation() const;
    void setCircularPresetNavigation(bool circular);

    uint8_t getNumberOfFilledPresets() const;
    void setNumberOfFilledPresets(uint8_t numberOfFilledPresets);

  private:
    bool footSwitchPolarityNormallyOpen_;
    uint16_t footSwitchLongPressTime_;
    bool circularPresetNavigation_;
    uint8_t numberOfFilledPresets_;
};

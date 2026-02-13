#pragma once

#include "dmx_presets.hpp"
class Messages {
  public:
    enum EventType {
        REQUEST_CONFIGURATION,
        CONFIGURATION_RESPONSE,
        SET_CONFIGURATION,
        SET_CONFIGURATION_RESPONSE,
        REQUEST_PRESETS,
        PRESETS_RESPONSE,
        SET_PRESETS,
        SET_PRESETS_RESPONSE
    };

    struct ConfigurationEventData {
        bool switch_polarity_inverted;
        uint16_t long_press_threshold_ms;
    };

    struct PresetEventData {
        uint8_t universe_1_data[512];
        uint16_t universe_1_length;
        uint8_t universe_2_data[512];
        uint16_t universe_2_length;
    };
    struct PresetsEventData {
        uint8_t number_of_presets;
        PresetEventData presets[DmxPresets::MAX_NR_OF_PRESETS];
    };

    struct Event {
        EventType type;
        union {
            ConfigurationEventData configurationData;
            PresetsEventData presetsData;
        } data;
    };
};
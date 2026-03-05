
#pragma once
#include <cstdint>

class Messages
{
  public:
    static const uint8_t MAX_NR_OF_PRESETS = 20;

    enum EventType
    {
        LOAD_CONFIGURATION,   // DMX Controller -> NVS Storage
        CONFIGURATION_LOADED, // NVS Storage -> DMX Controller
        UPDATE_CONFIGURATION, // DMX Controller -> Foot Switch, Web Server

        LOAD_DMX_PRESETS,   // DMX Controller -> NVS Storage
        DMX_PRESETS_LOADED, // NVS Storage -> DMX Controller
        UPDATE_DMX_PRESETS, // NVS Storage -> DMX Controller

        // OLD/TODO

        // Story: Foot switch for next/previous preset
        USER_NEXT_PRESET,          // Foot Switch -> DMX Controller
        USER_PREVIOUS_PRESET,      // Foot Switch -> DMX Controller
        SELECT_NEXT_PRESET,        // DMX Controller -> Preset Changer
        SELECT_PREVIOUS_PRESET,    // DMX Controller -> Preset Changer
        USE_PRESET_DATA,           // Preset Changer -> DMX Controller
        SEND_PRESET_DATA,          // DMX Controller -> Max3485 Sender
        SEND_PRESET_DATA_RESPONSE, // Max3485 Sender -> DMX Controller
        SHOW_PRESET_INDEX,         // DMX Controller -> Seven Segment Display

        // Added for unit test compatibility
        CONFIGURATION_RESPONSE,
        PRESETS_RESPONSE
    };

    struct ConfigurationEventData
    {
        bool switchPolarityNormallyOpen;
        uint16_t longPressThresholdMs;
    };

    struct PresetEventData
    {
        uint8_t presetNumber;
        const char *name;
        uint8_t *dmxValues;
    };

    struct PresetsEventData
    {
        uint8_t numberOfPresets;
        PresetEventData presets[MAX_NR_OF_PRESETS];
    };

    struct Event
    {
        EventType type;
        union
        {
        } data;
    };
};
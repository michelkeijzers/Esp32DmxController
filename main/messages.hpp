#pragma once

class Messages
{
  public:
    static const uint8_t MAX_NR_OF_PRESETS = 20;

    enum EventType
    {
        // Initialization
        REQUEST_CONFIGURATION,      // DMX Controller -> NVS Storage
        CONFIGURATION_RESPONSE,     // NVS Storage -> DMX Controller
        SET_CONFIGURATION,          // DMX Controller -> Foot Switch
        SET_CONFIGURATION_RESPONSE, // Foot Switch -> DMX Controller
        REQUEST_PRESETS,            // DMX Controller -> Preset Changer
        PRESETS_RESPONSE,           // Preset Changer -> DMX Controller
        SET_PRESETS,
        SET_PRESETS_RESPONSE,

        // Story: Foot switch for next/previous preset
        USER_NEXT_PRESET,          // Foot Switch -> DMX Controller
        USER_PREVIOUS_PRESET,      // Foot Switch -> DMX Controller
        SELECT_NEXT_PRESET,        // DMX Controller -> Preset Changer
        SELECT_PREVIOUS_PRESET,    // DMX Controller -> Preset Changer
        USE_PRESET_DATA,           // Preset Changer -> DMX Controller
        SEND_PRESET_DATA,          // DMX Controller -> Art-Net Sender
        SEND_PRESET_DATA_RESPONSE, // Art-Net Sender -> DMX Controller
        SHOW_PRESET_INDEX          // DMX Controller -> Seven Segment Display

    };

    struct ConfigurationEventData
    {
        bool switchPolarityInverted;
        uint16_t longPressThresholdMs;
    };

    struct PresetEventData
    {
        uint8_t presetNumber;
        const char *name;
        uint8_t universe1Data[512];
        uint16_t universe1Length;
        uint8_t universe2Data[512];
        uint16_t universe2Length;
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
            ConfigurationEventData configurationData;
            PresetsEventData presetsData;
            PresetEventData presetData;
        } data;
    };
};
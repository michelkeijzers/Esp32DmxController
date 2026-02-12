#ifndef NVS_STORAGE_HPP
#define NVS_STORAGE_HPP

#include <nvs.h>
#include <esp_err.h>
#include <string>
#include <vector>

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
}
#include "rtos_task.hpp"
#include "dmx_presets.hpp"

class NVSStorage : RtosTask
{
public:
    enum EventType
    {
        STORE_CONFIGURATION,
        GET_CONFIGURATION,
        STORE_PRESETS,
        GET_PRESETS
    };

    struct ConfigurationEventData
    {
        bool switch_polarity_inverted;
        uint16_t long_press_threshold_ms;
    };

    struct PresetEventData
    {
        uint8_t universe_1_data[512];
        uint16_t universe_1_length;
        uint8_t universe_2_data[512];
        uint16_t universe_2_length;
    };
    struct PresetsEventData
    {
        uint8_t number_of_presets;
        PresetEventData presets[MAX_NR_OF_PRESETS];
    };

    struct Event
    {
        EventType type;
        union
        {
            ConfigurationEventData configurationData;
            PresetsEventData presetsData;
        } data;
    };

    NVSStorage();
    ~NVSStorage();

    esp_err_t init();

    // Synchronous wrappers (for compatibility)
    esp_err_t storeConfiguration(const ConfigurationEventData &config);
    esp_err_t getConfiguration(ConfigurationEventData &config);
    esp_err_t storePresetsData(const PresetsEventData &presets);
    esp_err_t getPresetsData(PresetsEventData &presets);

private:
    nvs_handle_t configuration_nvs_handle;
    nvs_handle_t presets_nvs_handle;
    const char *configuration_namespace_name;
    const char *presets_namespace_name;

    void taskEntry(void *param) override;
    void taskLoop();
};

#endif // NVS_STORAGE_HPP
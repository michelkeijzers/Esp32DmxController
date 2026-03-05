#include "nv_storage.hpp"

#include "../Base/assert.hpp"
#include <cstring>
#include <esp_log.h>

NvStorage::NvStorage(Configuration &configuration, DmxPresets &dmxPresets)
    : RtosTask(), configurationNvsHandle_(0), presetsNvsHandle_(0), configurationNamespaceName_("configuration"),
      presetsNamespaceName_("presets"), configuration_(configuration), dmxPresets_(dmxPresets)
{
}

NvStorage::~NvStorage()
{
    if (configurationNvsHandle_ != 0)
    {
        nvs_close(configurationNvsHandle_);
    }

    if (presetsNvsHandle_ != 0)
    {
        nvs_close(presetsNvsHandle_);
    }
}

void NvStorage::init(RtosTask::TaskProperties taskProperties)
{
    RtosTask::init(taskProperties);
    Assert::assertNotEspError(nvs_open(configurationNamespaceName_, NVS_READWRITE, &configurationNvsHandle_),
        "Failed to open configuration NVS");
    Assert::assertNotEspError(
        nvs_open(presetsNamespaceName_, NVS_READWRITE, &presetsNvsHandle_), "Failed to open presets NVS");
}

void NvStorage::taskEntry(void *param) { static_cast<NvStorage *>(param)->taskLoop(); }

void NvStorage::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(logTag_, "NvStorage event received: %d", event.type);
            switch (event.type)
            {
            case Messages::EventType::LOAD_CONFIGURATION:
                loadConfiguration();
                break;

            case Messages::EventType::LOAD_DMX_PRESETS:
                loadDmxPresets();
                break;
            default:
                Assert::assertSoftwareError("Unknown NvStorage event type");
                break;
            }
        }
    }
}

// void NvStorage::saveConfiguration(Configuration &configurationData)
// {
//     assertNot0(configurationNvsHandle_, "configurationNvsHandle_");
//     Assert::assertNotEspError(nvs_set_u8(
//         configurationNvsHandle_, "SwitchPolarityNormallyOpen", configurationData.switchPolarityNormallyOpen), "Failed
//         to set SwitchPolarityNormallyOpen");
//     Assert::assertNotEspError(
//         nvs_set_u16(configurationNvsHandle_, "LongPressThreshold", configurationData.longPressThresholdMs), "Failed
//         to set LongPressThreshold");
//     Assert::assertNotEspError(nvs_commit(configurationNvsHandle_), "Failed to commit configuration");
// }

void NvStorage::loadConfiguration()
{
    Assert::assertNot0(configurationNvsHandle_, "configurationNvsHandle_");

    configuration_.lock();

    uint8_t footSwitchPolarityNormallyOpen;
    Assert::assertNotEspError(
        nvs_get_u8(configurationNvsHandle_, "FootSwitchPolarityNormallyOpen", &footSwitchPolarityNormallyOpen),
        "Failed to get FootSwitchPolarityNormallyOpen");
    configuration_.setFootSwitchPolarityNormallyOpen(footSwitchPolarityNormallyOpen);

    uint16_t footSwitchLongPressTime_;
    Assert::assertNotEspError(
        nvs_get_u16(configurationNvsHandle_, "FootSwitchLongPressThreshold", &footSwitchLongPressTime_),
        "Failed to get FootSwitchLongPressThreshold");
    configuration_.setFootSwitchLongPressTime(footSwitchLongPressTime_);

    uint8_t numberOfFilledPresets = 0;
    Assert::assertNotEspError(nvs_get_u8(configurationNvsHandle_, "NumberOfFilledPresets", &numberOfFilledPresets),
        "Failed to get NumberOfFilledPresets");
    configuration_.setNumberOfFilledPresets(numberOfFilledPresets);

    uint8_t circularPresetNavigation;
    Assert::assertNotEspError(
        nvs_get_u8(configurationNvsHandle_, "CircularPresetNavigation", &circularPresetNavigation),
        "Failed to get CircularPresetNavigation");
    configuration_.setCircularPresetNavigation(circularPresetNavigation);

    configuration_.unlock();

    Messages::Event responseEvent;
    responseEvent.type = Messages::CONFIGURATION_LOADED;
    xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);
}

// void NvStorage::setPresets(const Messages::PresetsEventData &presetsData)
// {
//     assertNot0(presetsNvsHandle_, "presetsNvsHandle_");

//     Assert::assertNotEspError(nvs_set_u8(presetsNvsHandle_, "NumberOfPresets", presetsData.numberOfPresets), "Failed
//     to set NumberOfPresets");

//     for (uint8_t i = 0; i < presetsData.numberOfPresets; ++i)
//     {
//         const Messages::PresetEventData &preset = presetsData.presets[i];
//         char key[16];
//         snprintf(key, sizeof(key), "Preset%d", i);
//         Assert::assertNotEspError(nvs_set_blob(presetsNvsHandle_, key, &preset, sizeof(Messages::PresetEventData)),
//         "Failed to set Preset blob");
//     }

//     Assert::assertNotEspError(nvs_commit(presetsNvsHandle_), "Failed to commit presets");
// }

void NvStorage::loadDmxPresets()
{
    Assert::assertNvsHandle(presetsNvsHandle_, "presetsNvsHandle_");

    dmxPresets_.lock();

    uint8_t numberOfFilledPresets = 0;
    Assert::assertNotEspError(nvs_get_u8(presetsNvsHandle_, "NumberOfFilledPresets", &numberOfFilledPresets),
        "Failed to get NumberOfFilledPresets");
    dmxPresets_.setNumberOfFilledPresets(numberOfFilledPresets);

    for (uint8_t presetIndex = 0; presetIndex < numberOfFilledPresets; presetIndex++)
    {
        char key[15]; // Including null char.
        DmxPreset &dmxPreset = dmxPresets_.getPreset(presetIndex);
        dmxPreset.setIndex(presetIndex + 1);

        snprintf(key, sizeof(key), "P%d.Name", presetIndex);
        size_t length = 32; // TODO: Use max length of name
        char name[32];
        Assert::assertNotEspError(nvs_get_str(presetsNvsHandle_, key, name, &length), "Failed to get Preset name");
        dmxPreset.setName(name);

        snprintf(key, sizeof(key), "P%d.Values", presetIndex);
        length = 512; // TODO: Use actual size of DMX values
        uint8_t dmxValues[512];
        Assert::assertNotEspError(
            nvs_get_blob(presetsNvsHandle_, key, dmxValues, &length), "Failed to get Preset DMX values");
        dmxPreset.setDmxValues(dmxValues);
    }

    dmxPresets_.unlock();

    Messages::Event responseEvent;
    responseEvent.type = Messages::DMX_PRESETS_LOADED;
    xQueueSend(getDmxControllerEventQueue(), &responseEvent, portMAX_DELAY);
}

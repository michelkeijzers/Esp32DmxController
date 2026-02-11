#include "dmx_preset_changer.hpp"
#include <esp_log.h>

static const char *TAG = "DmxPresetChanger";

DmxPresetChanger::DmxPresetChanger(DmxPresets *presets,
                                   OSCSender *osc,
                                   SevenSegmentDisplay *display,
                                   ArtNetSender *artnet)
    : dmxPresets_(presets), oscSender_(osc), display_(display), artnetSender_(artnet),
      taskHandle_(nullptr), eventQueue_(nullptr)
{
    eventQueue_ = xQueueCreate(4, sizeof(EventType));
    if (eventQueue_)
    {
        xTaskCreate(taskEntry, "DmxPresetChangerTask", 4096, this, 5, &taskHandle_);
        ESP_LOGI(TAG, "DMX Preset Changer task started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create DmxPresetChanger event queue");
    }
}

DmxPresetChanger::~DmxPresetChanger()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    if (eventQueue_)
    {
        vQueueDelete(eventQueue_);
    }
}

void DmxPresetChanger::init()
{
    // TODO any initialization if needed
}

void DmxPresetChanger::postEvent(EventType event)
{
    if (eventQueue_)
    {
        xQueueSend(eventQueue_, &event, 0);
    }
}

void DmxPresetChanger::taskEntry(void *param)
{
    static_cast<DmxPresetChanger *>(param)->taskLoop();
}

void DmxPresetChanger::taskLoop()
{
    EventType event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event)
            {
            case SHORT_PRESS:
                handleShortPress();
                break;
            case LONG_PRESS:
                handleLongPress();
                break;
            }
        }
    }
}

void DmxPresetChanger::setDmxPresets(DmxPresets *presets)
{
    dmxPresets_ = presets;
}

void DmxPresetChanger::setOSCSender(OSCSender *osc)
{
    oscSender_ = osc;
}

void DmxPresetChanger::setDisplay(SevenSegmentDisplay *display)
{
    display_ = display;
}

void DmxPresetChanger::setArtNetSender(ArtNetSender *artnet)
{
    artnetSender_ = artnet;
}

void DmxPresetChanger::handleShortPress()
{
    if (!dmxPresets_)
    {
        ESP_LOGE(TAG, "No DMX presets configured");
        return;
    }

    uint8_t newPreset = dmxPresets_->nextPreset();
    ESP_LOGI(TAG, "Short press: Next preset - Preset %d", newPreset);

    // Update display
    updateDisplay();

    // Send OSC message for preset change
    if (oscSender_)
    {
        oscSender_->sendMessage("/dmx/preset", static_cast<int32_t>(newPreset));
        ESP_LOGD(TAG, "OSC message sent: /dmx/preset %d", newPreset);
    }

    // Send DMX data via Art-Net
    sendCurrentPresetToArtNet();
}

void DmxPresetChanger::handleLongPress()
{
    if (!dmxPresets_)
    {
        ESP_LOGE(TAG, "No DMX presets configured");
        return;
    }

    uint8_t newPreset = dmxPresets_->previousPreset();
    ESP_LOGI(TAG, "Long press: Previous preset - Preset %d", newPreset);

    // Update display
    updateDisplay();

    // Send OSC message for preset change
    if (oscSender_)
    {
        oscSender_->sendMessage("/dmx/preset", static_cast<int32_t>(newPreset));
        ESP_LOGD(TAG, "OSC message sent: /dmx/preset %d", newPreset);
    }

    // Send DMX data via Art-Net
    sendCurrentPresetToArtNet();
}

void DmxPresetChanger::updateDisplay()
{
    if (!display_ || !dmxPresets_)
    {
        return;
    }

    uint8_t currentPreset = dmxPresets_->getCurrentPresetIndex();

    // Display logic: if preset >= 10, show (preset-10) with decimal point
    uint8_t displayDigit = currentPreset;
    bool showDecimalPoint = false;

    if (currentPreset >= 10)
    {
        displayDigit = currentPreset - 10;
        showDecimalPoint = true;
    }

    // TODO display_->displayDigit(displayDigit, showDecimalPoint);
    ESP_LOGD(TAG, "Display updated: digit=%d, decimal_point=%d", displayDigit, showDecimalPoint);
}

void DmxPresetChanger::sendCurrentPresetToArtNet()
{
    if (!artnetSender_ || !dmxPresets_)
    {
        return;
    }

    uint8_t currentPreset = dmxPresets_->getCurrentPresetIndex();
    const DmxPreset *preset = dmxPresets_->getPreset(currentPreset);

    if (!preset)
    {
        ESP_LOGE(TAG, "Failed to get preset %d", currentPreset);
        return;
    }

    // Send both universes
    esp_err_t ret = artnetSender_->sendUniverses(
        preset->getUniverseData(0), DMX_UNIVERSE_SIZE,
        preset->getUniverseData(1), DMX_UNIVERSE_SIZE);

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Sent preset %d to Art-Net", currentPreset);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send preset %d to Art-Net", currentPreset);
    }
}
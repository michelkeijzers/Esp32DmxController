#pragma once

#include <stdio.h>
#include "osc_sender.hpp"
#include "dmx_presets.hpp"
#include "seven_segment_display.hpp"
#include "artnet_sender.hpp"

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
}

class DmxPresetChanger
{
public:
    enum EventType
    {
        SHORT_PRESS,
        LONG_PRESS
    };

    DmxPresetChanger(DmxPresets *presets = nullptr,
                     OSCSender *osc = nullptr,
                     SevenSegmentDisplay *display = nullptr,
                     ArtNetSender *artnet = nullptr);
    ~DmxPresetChanger();

    void init();

    void setDmxPresets(DmxPresets *presets);
    void setOSCSender(OSCSender *osc);
    void setDisplay(SevenSegmentDisplay *display);
    void setArtNetSender(ArtNetSender *artnet);

    // Post an event to the task
    void postEvent(EventType event);

private:
    DmxPresets *dmxPresets_;
    OSCSender *oscSender_;
    SevenSegmentDisplay *display_;
    ArtNetSender *artnetSender_;

    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;

    static void taskEntry(void *param);
    void taskLoop();

    void handleShortPress();
    void handleLongPress();
    void updateDisplay();
    void sendCurrentPresetToArtNet();
};
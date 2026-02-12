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
#include "rtos_task.hpp"
#include "dmx_presets.hpp"

class DmxPresetChanger : RtosTask
{
public:
    enum EventType
    {
        PREVIOUS_PRESET,
        NEXT_PRESET
    };

    struct Event
    {
        EventType type;
    };

    DmxPresetChanger();
    ~DmxPresetChanger();

    esp_err_t init();

private:
    DmxPresets dmxPresets_;

    void taskEntry(void *param) override;
    void taskLoop();
};
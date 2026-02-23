#pragma once

#include "../Data/dmx_presets.hpp"
#include "artnet_sender.hpp"
#include "osc_sender.hpp"
#include "seven_segment_display.hpp"
#include <stdio.h>

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "../Data/dmx_presets.hpp"
#include "Base/rtos_task.hpp"
#include "messages.hpp"

class DmxPresetChanger : public RtosTask
{
  public:
    DmxPresetChanger();
    ~DmxPresetChanger();

    virtual esp_err_t init(QueueHandle_t dmxControllerEventQueue);

  private:
    DmxPresets dmxPresets_;

    void taskEntry(void *param) override;
    void taskLoop();

    void setPresets(const Messages::PresetsEventData &presetsData);
};
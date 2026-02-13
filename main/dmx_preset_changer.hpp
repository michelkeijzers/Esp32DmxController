#pragma once

#include "artnet_sender.hpp"
#include "dmx_presets.hpp"
#include "osc_sender.hpp"
#include "seven_segment_display.hpp"
#include <stdio.h>

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "dmx_presets.hpp"
#include "messages.hpp"
#include "rtos_task.hpp"

class DmxPresetChanger : public RtosTask {
  public:
    DmxPresetChanger();
    ~DmxPresetChanger();

    esp_err_t init(QueueHandle_t dmxControllerEventQueue);

  private:
    DmxPresets dmxPresets_;

    void taskEntry(void *param) override;
    void taskLoop();

    void setPresets(const Messages::PresetsEventData &presetsData);
};
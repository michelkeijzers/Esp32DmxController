#pragma once

#include "../Data/dmx_presets.hpp"
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

    virtual esp_err_t init(RtosTask::TaskProperties taskProperties);

  private:
    DmxPresets dmxPresets_;

    void taskEntry(void *param) override;
    void taskLoop();

    void setPresets(const Messages::PresetsEventData &presetsData);
};
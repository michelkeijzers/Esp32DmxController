#pragma once
#include "../Base/rtos_task.hpp"
#include "../Data/dmx_presets.hpp"
#include "driver/gpio.h"
#include "foot_switch.hpp"
#include "max3485_sender.hpp"
#include "nv_storage.hpp"
#include "seven_segment_display.hpp"
#include "web_server.hpp"
#include <../Data/configuration.hpp>
#include <esp_err.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_wifi.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <stdint.h>
#include <stdio.h>

class DmxController : public RtosTask
{
  public:
    DmxController(Configuration &configuration, DmxPresets &dmxPresets, SevenSegmentDisplay *display,
        FootSwitch *footSwitch, Max3485Sender *dmx3485Sender, WebServer *webServer, NvStorage *nvStorage);
    ~DmxController();
    virtual void init();
    void taskLoop();

  private:
    void initSubTasks();
    void initMessages();

  protected:
    static constexpr gpio_num_t FOOT_SWITCH_PIN = GPIO_NUM_4;
    static constexpr gpio_num_t DISPLAY_PINS[8] = {
        GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9};
    static constexpr const char *OTA_URL = "https://example.com/firmware.bin";
    static constexpr const char *OTA_CERT_PEM = nullptr;

    SevenSegmentDisplay *display_;
    FootSwitch *footSwitch_;
    Max3485Sender *max3485Sender_;
    WebServer *webServer_;
    NvStorage *nvStorage_;

    Configuration configuration_;
    DmxPresets dmxPresets_;

    TickType_t bootTime = 0;
    void taskEntry(void *param) override;
    esp_err_t performOtaUpdate(const char *url);
    void logFirmwareInfo();
};

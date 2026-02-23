#pragma once
#include "Base/rtos_task.hpp"
#include "Data/dmx_presets.hpp"
#include "artnet_sender.hpp"
#include "dmx_preset_changer.hpp"
#include "driver/gpio.h"
#include "foot_switch.hpp"
#include "max3485_sender.hpp"
#include "nvs_storage.hpp"
#include "osc_sender.hpp"
#include "seven_segment_display.hpp"
#include "web_server.hpp"
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
    DmxController(DmxPresetChanger *presetChanger = nullptr, OSCSender *oscSender = nullptr,
        SevenSegmentDisplay *display = nullptr, FootSwitch *footSwitch = nullptr,
        Max3485Sender *dmx3485Sender = nullptr, ArtNetSender *artnetSender = nullptr, WebServer *webServer = nullptr,
        NvsStorage *nvsStorage = nullptr);
    ~DmxController();
    esp_err_t init();
    void taskLoop();

  private:
    esp_err_t init_sub_tasks();
    esp_err_t init_messages();
    esp_err_t performOtaUpdate(const char *url);
    void printFirmwareInfo();

  protected:
    static constexpr gpio_num_t FOOT_SWITCH_PIN = GPIO_NUM_4;
    static constexpr gpio_num_t DISPLAY_PINS[8] = {
        GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9};
    static constexpr const char *OTA_URL = "https://example.com/firmware.bin";
    static constexpr const char *OTA_CERT_PEM = nullptr;
    static constexpr const char *OSC_DEST_IP = "192.168.1.100";
    static constexpr int OSC_DEST_PORT = 8000;
    static constexpr const char *ARTNET_DEST_IP = "192.168.1.100";

    DmxPresetChanger *presetChanger_ = nullptr;
    OSCSender *oscSender_ = nullptr;
    SevenSegmentDisplay *display_ = nullptr;
    FootSwitch *footSwitch_ = nullptr;
    Max3485Sender *max3485Sender_ = nullptr;
    ArtNetSender *artnetSender_ = nullptr;
    WebServer *webServer_ = nullptr;
    NvsStorage *nvsStorage_ = nullptr;

    TickType_t bootTime = 0;

    void taskEntry(void *param) override;
};

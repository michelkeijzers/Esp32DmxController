#pragma once
#include <stdio.h>
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include "foot_switch.hpp"
#include "dmx_preset_changer.hpp"
#include <esp_https_ota.h>
#include <esp_ota_ops.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include "osc_sender.hpp"
#include "dmx_presets.hpp"
#include "seven_segment_display.hpp"
#include "artnet_sender.hpp"
#include "web_server.hpp"
#include "rtos_task.hpp"

class DmxController : RtosTask
{
public:
    struct DmxControllerEvent
    {
        // TODO Config, Prev preset, next preset etc
    };

    DmxController();
    ~DmxController();
    esp_err_t init();
    void taskLoop();
    esp_err_t performOtaUpdate(const char *url);
    void printFirmwareInfo();

private:
    static constexpr gpio_num_t FOOT_SWITCH_PIN = GPIO_NUM_4;
    static constexpr gpio_num_t DISPLAY_PINS[8] = {
        GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_5,
        GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9};
    static constexpr const char *OTA_URL = "https://example.com/firmware.bin";
    static constexpr const char *OTA_CERT_PEM = nullptr;
    static constexpr const char *OSC_DEST_IP = "192.168.1.100";
    static constexpr int OSC_DEST_PORT = 8000;
    static constexpr const char *ARTNET_DEST_IP = "192.168.1.100";

    DmxPresetChanger *presetChanger = nullptr;
    OSCSender *oscSender = nullptr;
    DmxPresets *dmxPresets = nullptr;
    SevenSegmentDisplay *display = nullptr;
    FootSwitch *footSwitch = nullptr;
    ArtNetSender *artnetSender = nullptr;
    WebServer *webServer = nullptr;
    TickType_t bootTime = 0;

    void taskEntry(void *param) override;
};

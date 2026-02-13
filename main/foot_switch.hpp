#pragma once

#include "rtos_task.hpp"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

enum InterruptEventType { PRESS, RELEASE };

class FootSwitch : public RtosTask {
  public:
    struct InterruptEvent {
        InterruptEventType type;
    };

    enum class State { BOOT, OTA_CHECK, OTA, NORMAL_OPERATION };

    FootSwitch();
    ~FootSwitch();

    esp_err_t init(QueueHandle_t dmxControllerEventQueue, gpio_num_t pinNum);

    uint16_t getLongPressThresholdMs();
    bool getPolarityInverted();

    void taskEntry(void *param) override;

    gpio_num_t getPin() const { return pin_; }

  private:
    gpio_num_t pin_;

    bool lastPinState_;
    TickType_t pressStartTime_;
    uint32_t longPressTimeMs_;

    bool polarityInverted_;
    uint16_t longPressThresholdMs_;

    State state_;

    void taskLoop();

    esp_err_t HandleShortPress();
    esp_err_t HandleLongPress();
};
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

    struct Event {
        // TODO: configuring long press threshold, polarity etc
    };

    enum class State { BOOT, OTA_CHECK, OTA, NORMAL_OPERATION };

    FootSwitch();
    ~FootSwitch();

    esp_err_t init(QueueHandle_t dmxControllerEventQueue, gpio_num_t pinNum);

    uint16_t getLongPressThresholdMs();
    bool getPolarityInverted();

    void taskEntry(void *param) override;

    gpio_num_t getPin() const { return _pin; }

  private:
    gpio_num_t _pin;

    bool lastPinState;
    TickType_t pressStartTime;
    uint32_t longPressTimeMs;
    bool polarityInverted;         // Configuration
    uint16_t longPressThresholdMs; // Configuration

    State state_;

    void taskLoop();

    bool HandleShortPress();
    bool HandleLongPress();
};
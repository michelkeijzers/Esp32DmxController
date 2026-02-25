#pragma once

#include "Base/rtos_task.hpp"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

enum InterruptEventType
{
    PRESS,
    RELEASE
};

class FootSwitch : public RtosTask
{
  public:
    struct InterruptEvent
    {
        InterruptEventType type;
    };

    FootSwitch();
    ~FootSwitch();

    virtual esp_err_t init(RtosTask::TaskProperties taskProperties, gpio_num_t pinNum);

  private:
    const char *logTag_;
    int taskPriority_;
    int queueCapacity_;

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

    void taskLoop();

    esp_err_t HandleShortPress();
    esp_err_t HandleLongPress();
};
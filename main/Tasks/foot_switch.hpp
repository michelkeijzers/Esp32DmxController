#pragma once

#include "../Data/configuration.hpp"
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

    FootSwitch(Configuration &configuration);
    ~FootSwitch();

    virtual void init(RtosTask::TaskProperties taskProperties, gpio_num_t pinNum);

    const char *logTag_;
    int taskPriority_;
    int queueCapacity_;

    uint16_t getLongPressThresholdMs();
    bool getPolarityNormallyOpen();

    void taskEntry(void *param) override;

  public:
    gpio_num_t getPin() const { return pin_; }

  private:
    Configuration &configuration_;

    gpio_num_t pin_;

    bool lastPinState_;
    TickType_t pressStartTime_;
    uint32_t longPressTimeMs_;

    bool polarityNormallyOpen_;
    uint16_t longPressThresholdMs_;

    void taskLoop();

    esp_err_t HandleShortPress();
    esp_err_t HandleLongPress();
};
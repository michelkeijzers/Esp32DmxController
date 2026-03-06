// foot_switch.hpp
#pragma once

#include "../Data/configuration.hpp"
#include "Base/rtos_task.hpp"
#include <../Base/assert.hpp>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

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
        TickType_t timestamp;
    };

    enum class State
    {
        IDLE,             // Released and stable
        DEBOUNCE_PRESS,   // Detected press, waiting for stability
        PRESSED,          // Press confirmed, waiting for release
        DEBOUNCE_RELEASE, // Detected release, waiting for stability
        RELEASED          // Release confirmed, evaluating duration
    };

    FootSwitch(IAssert *assert, Configuration &configuration);
    ~FootSwitch();

    virtual void init(RtosTask::TaskProperties taskProperties, gpio_num_t pinNum);

    int taskPriority_;
    int queueCapacity_;

    uint16_t getLongPressThresholdMs();
    bool getPolarityNormallyOpen();
    State getState() const { return state_; }

    void taskEntry(void *param) override;
    gpio_num_t getPin() const { return pin_; }

  private:
    Configuration &configuration_;
    IAssert *assert_;

    gpio_num_t pin_;

    // State machine
    State state_;
    TickType_t stateEntryTime_;

    // Timing
    TickType_t pressStartTime_;
    uint32_t debounceDelayMs_;
    uint32_t longPressThresholdMs_;

    // Configuration
    bool polarityNormallyOpen_;

    // State machine methods
    void transitionTo(State newState);
    const char *stateToString(State state) const;

    // Event handlers
    void handleInterruptEvent(const InterruptEvent &event);
    void handleTimeout();
    void handleConfigUpdate(QueueHandle_t configEventQueue);
    TickType_t calculateBlockTime() const;

    void taskLoop();

    esp_err_t HandleShortPress();
    esp_err_t HandleLongPress();
};
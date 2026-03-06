// foot_switch.cpp
#ifdef _MSC_VER
#ifndef portYIELD_FROM_ISR
#define portYIELD_FROM_ISR() ((void)0)
#endif
#endif

#include "foot_switch.hpp"
#include "../Base/assert.hpp"
#include "esp_attr.h"
#include "messages.hpp"
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <stdexcept>
#include <stdio.h>

static const char *TAG = "FootSwitch";
static const int QUEUE_CAPACITY = 10;
static QueueHandle_t interruptEventQueue = nullptr;
static QueueHandle_t configEventQueue = nullptr;

static gpio_num_t isr_pin = GPIO_NUM_NC;

// ISR handler: minimal, just post event to queue
static void IRAM_ATTR isr_handler(void *arg)
{
    int level = gpio_get_level(isr_pin);
    FootSwitch::InterruptEvent event;
    event.type = (level == 0) ? InterruptEventType::PRESS : InterruptEventType::RELEASE;
    event.timestamp = xTaskGetTickCountFromISR();

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(interruptEventQueue, &event, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

FootSwitch::FootSwitch(IAssert *assert, Configuration &configuration)
    : RtosTask(), configuration_(configuration), assert_(assert), pin_(GPIO_NUM_NC), state_(State::IDLE),
      stateEntryTime_(0), pressStartTime_(0), debounceDelayMs_(30), longPressThresholdMs_(1000),
      polarityNormallyOpen_(true)
{
}

FootSwitch::~FootSwitch() {}

void FootSwitch::init(RtosTask::TaskProperties taskProperties, gpio_num_t pinNum)
{
    RtosTask::init(taskProperties);

    // Configure GPIO
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << pinNum);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;

    assert_->assertNotEspError(gpio_config(&io_conf), "Failed to configure GPIO");

    pin_ = pinNum;
    isr_pin = pinNum;

    // Create interrupt event queue
    interruptEventQueue = xQueueCreate(QUEUE_CAPACITY, sizeof(FootSwitch::InterruptEvent));
    assert_->assertQueueHandle(interruptEventQueue, "interruptEventQueue");

    // Get config event queue
    configEventQueue = getEventQueue();

    // Install ISR
    assert_->assertNotEspError(gpio_install_isr_service(0), "Failed to install ISR service");
    assert_->assertNotEspError(gpio_isr_handler_add(pin_, isr_handler, this), "Failed to add ISR handler");

    // Load initial configuration
    configuration_.lock();
    polarityNormallyOpen_ = configuration_.getFootSwitchPolarityNormallyOpen();
    longPressThresholdMs_ = configuration_.getFootSwitchLongPressTime();
    configuration_.unlock();

    ESP_LOGI(TAG, "FootSwitch initialized: pin=%d, polarity=%d, longPressMs=%lu", pin_, polarityNormallyOpen_,
        longPressThresholdMs_);
}

const char *FootSwitch::stateToString(State state) const
{
    switch (state)
    {
    case State::IDLE:
        return "IDLE";
    case State::DEBOUNCE_PRESS:
        return "DEBOUNCE_PRESS";
    case State::PRESSED:
        return "PRESSED";
    case State::DEBOUNCE_RELEASE:
        return "DEBOUNCE_RELEASE";
    case State::RELEASED:
        return "RELEASED";
    default:
        return "UNKNOWN";
    }
}

void FootSwitch::transitionTo(State newState)
{
    if (state_ != newState)
    {
        ESP_LOGI(TAG, "State: %s -> %s", stateToString(state_), stateToString(newState));
        state_ = newState;
        stateEntryTime_ = xTaskGetTickCount();
    }
}

void FootSwitch::handleInterruptEvent(const InterruptEvent &event)
{
    bool isPress = (event.type == InterruptEventType::PRESS);

    switch (state_)
    {
    case State::IDLE:
        if (isPress)
        {
            transitionTo(State::DEBOUNCE_PRESS);
        }
        break;

    case State::DEBOUNCE_PRESS:
        if (isPress)
        {
            // Still bouncing press - reset debounce timer
            stateEntryTime_ = event.timestamp;
            ESP_LOGD(TAG, "Press bounce, timer reset");
        }
        else
        {
            // Bounced back to released
            ESP_LOGD(TAG, "Bounced back to released during press debounce");
            transitionTo(State::IDLE);
        }
        break;

    case State::PRESSED:
        if (!isPress)
        {
            transitionTo(State::DEBOUNCE_RELEASE);
        }
        break;

    case State::DEBOUNCE_RELEASE:
        if (!isPress)
        {
            // Still bouncing release - reset debounce timer
            stateEntryTime_ = event.timestamp;
            ESP_LOGD(TAG, "Release bounce, timer reset");
        }
        else
        {
            // Bounced back to pressed
            ESP_LOGD(TAG, "Bounced back to pressed during release debounce");
            transitionTo(State::PRESSED);
        }
        break;

    case State::RELEASED:
        // Transient state - should transition to IDLE quickly
        if (isPress)
        {
            transitionTo(State::DEBOUNCE_PRESS);
        }
        break;
    }
}

void FootSwitch::handleTimeout()
{
    TickType_t now = xTaskGetTickCount();
    TickType_t timeInState = (now - stateEntryTime_) * portTICK_PERIOD_MS;

    switch (state_)
    {
    case State::DEBOUNCE_PRESS:
        if (timeInState >= debounceDelayMs_)
        {
            // Debounce complete - press confirmed
            pressStartTime_ = now;
            transitionTo(State::PRESSED);
            ESP_LOGI(TAG, "Press confirmed");
        }
        break;

    case State::DEBOUNCE_RELEASE:
        if (timeInState >= debounceDelayMs_)
        {
            // Debounce complete - release confirmed
            transitionTo(State::RELEASED);

            // Calculate press duration
            TickType_t pressDuration = (now - pressStartTime_) * portTICK_PERIOD_MS;

            ESP_LOGI(TAG, "Release confirmed, duration: %lu ms", pressDuration);

            // Determine press type and handle
            if (pressDuration >= longPressThresholdMs_)
            {
                ESP_LOGI(TAG, "Long press: %lu ms", pressDuration);
                if (HandleLongPress() != ESP_OK)
                {
                    ESP_LOGW(TAG, "Long press rejected");
                }
            }
            else
            {
                ESP_LOGI(TAG, "Short press: %lu ms", pressDuration);
                if (HandleShortPress() != ESP_OK)
                {
                    ESP_LOGW(TAG, "Short press rejected");
                }
            }

            // Return to idle
            transitionTo(State::IDLE);
        }
        break;

    case State::IDLE:
    case State::PRESSED:
    case State::RELEASED:
        // No timeout handling needed
        break;
    }
}

void FootSwitch::handleConfigUpdate(QueueHandle_t configQueue)
{
    Messages::Event configEvent;

    while (xQueueReceive(configQueue, &configEvent, 0) == pdTRUE)
    {
        if (configEvent.type == Messages::EventType::UPDATE_CONFIGURATION)
        {
            configuration_.lock();
            polarityNormallyOpen_ = configuration_.getFootSwitchPolarityNormallyOpen();
            longPressThresholdMs_ = configuration_.getFootSwitchLongPressTime();
            configuration_.unlock();

            ESP_LOGI(TAG, "Config updated: polarity=%d, longPressMs=%lu", polarityNormallyOpen_, longPressThresholdMs_);
        }
        else
        {
            ESP_LOGW(TAG, "Unknown config event type: %d", static_cast<int>(configEvent.type));
            throw std::runtime_error("Unknown config event type");
        }
    }
}

TickType_t FootSwitch::calculateBlockTime() const
{
    if (state_ == State::DEBOUNCE_PRESS || state_ == State::DEBOUNCE_RELEASE)
    {
        // Calculate remaining time until debounce completes
        TickType_t now = xTaskGetTickCount();
        TickType_t timeInState = (now - stateEntryTime_) * portTICK_PERIOD_MS;

        if (timeInState >= debounceDelayMs_)
        {
            return 0; // Debounce period already elapsed, process immediately
        }
        else
        {
            TickType_t remaining = debounceDelayMs_ - timeInState;
            return pdMS_TO_TICKS(remaining + 1); // +1 to ensure we exceed threshold
        }
    }
    else
    {
        // Not debouncing - block indefinitely
        return portMAX_DELAY;
    }
}

void FootSwitch::taskEntry(void *param) { static_cast<FootSwitch *>(param)->taskLoop(); }

void FootSwitch::taskLoop()
{
    InterruptEvent interruptEvent;

    ESP_LOGI(TAG, "Task started, initial state: %s", stateToString(state_));

    while (true)
    {
        // Calculate how long to block based on current state
        TickType_t blockTime = calculateBlockTime();

        // Wait for interrupt event (or timeout)
        BaseType_t received = xQueueReceive(interruptEventQueue, &interruptEvent, blockTime);

        if (received == pdTRUE)
        {
            // Interrupt event received
            ESP_LOGD(TAG, "Interrupt: %s at %lu ms",
                interruptEvent.type == InterruptEventType::PRESS ? "PRESS" : "RELEASE",
                interruptEvent.timestamp * portTICK_PERIOD_MS);

            handleInterruptEvent(interruptEvent);

            // Drain any additional events (process latest state)
            while (xQueueReceive(interruptEventQueue, &interruptEvent, 0) == pdTRUE)
            {
                ESP_LOGD(TAG, "Draining: %s", interruptEvent.type == InterruptEventType::PRESS ? "PRESS" : "RELEASE");
                handleInterruptEvent(interruptEvent);
            }
        }
        else
        {
            // Timeout - check if debounce period complete
            handleTimeout();
        }

        // Check for configuration updates (non-blocking)
        handleConfigUpdate(configEventQueue);
    }
}

esp_err_t FootSwitch::HandleShortPress()
{
    ESP_LOGI(TAG, "Short press action");

    Messages::Event event;
    event.type = Messages::USER_NEXT_PRESET;

    if (xQueueSend(getDmxControllerEventQueue(), &event, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to send USER_NEXT_PRESET event to DMX Controller");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t FootSwitch::HandleLongPress()
{
    ESP_LOGI(TAG, "Long press action");

    Messages::Event event;
    event.type = Messages::USER_PREVIOUS_PRESET;

    if (xQueueSend(getDmxControllerEventQueue(), &event, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to send USER_PREVIOUS_PRESET event to DMX Controller");
        return ESP_FAIL;
    }

    return ESP_OK;
}

uint16_t FootSwitch::getLongPressThresholdMs() { return longPressThresholdMs_; }

bool FootSwitch::getPolarityNormallyOpen() { return polarityNormallyOpen_; }
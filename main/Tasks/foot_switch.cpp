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
#include <stdio.h>

static const int QUEUE_CAPACITY = 10;
static QueueHandle_t interruptEventQueue = nullptr;
static QueueHandle_t configEventQueue = nullptr;

static gpio_num_t isr_pin = GPIO_NUM_NC;

// ISR handler: minimal, just post event to queue
static void IRAM_ATTR isr_handler(void *arg)
{
    // FootSwitch *footSwitch = static_cast<FootSwitch *>(arg); // TODO: UNUSED VARIABLE
    int level = gpio_get_level(isr_pin);
    FootSwitch::InterruptEvent event;
    event.type = (level == 0) ? InterruptEventType::PRESS : InterruptEventType::RELEASE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(interruptEventQueue, &event, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

FootSwitch::FootSwitch(Configuration &configuration)
    : RtosTask(), configuration_(configuration), pin_(GPIO_NUM_NC), lastPinState_(false), pressStartTime_(0),
      longPressTimeMs_(1000), // Default long press time
      polarityNormallyOpen_(true), longPressThresholdMs_(1000)
{
}

FootSwitch::~FootSwitch() {}

void FootSwitch::init(RtosTask::TaskProperties taskProperties, gpio_num_t pinNum)
{
    RtosTask::init(taskProperties);

    // Classic C++ initialization for unit test build
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << pinNum);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_ANYEDGE; // Enable interrupt on both edges

    Assert::assertNotEspError(gpio_config(&io_conf), "Failed to configure GPIO");
    int initialLevel = gpio_get_level(pinNum);
    lastPinState_ = (initialLevel == 0);

    pin_ = pinNum;
    isr_pin = pinNum;
    ESP_LOGI("1", "1");
    interruptEventQueue = xQueueCreate(QUEUE_CAPACITY, sizeof(FootSwitch::InterruptEvent));
    ESP_LOGI("1", "2");
    printf("%p\n", interruptEventQueue);
    Assert::assertQueueHandle(interruptEventQueue, "interruptEventQueue");
    ESP_LOGI("1", "3");
    configEventQueue = getEventQueue();
    ESP_LOGI("1", "4");

    Assert::assertNotEspError(gpio_install_isr_service(0), "Failed to install ISR service");
    Assert::assertNotEspError(gpio_isr_handler_add(pin_, isr_handler, this), "Failed to add ISR handler");

    ESP_LOGI("1", "5");
    ESP_LOGI(pcTaskGetName(nullptr), "FootSwitchTask task started (interrupt mode)");
    ESP_LOGI("1", "6");
}

void FootSwitch::taskEntry(void *param) { static_cast<FootSwitch *>(param)->taskLoop(); }

void FootSwitch::taskLoop()
{
    InterruptEvent interruptEvent;
    Messages::Event configEvent;
    const int debounceDelayMs = 30; // Debounce delay in ms
    bool debouncedState = false;
    bool lastStableState = false;
    TickType_t lastDebounceTime = 0;

    while (true)
    {
        if (xQueueReceive(interruptEventQueue, &interruptEvent, 0) == pdTRUE)
        {

            TickType_t now = xTaskGetTickCount();
            bool currentState = (interruptEvent.type == InterruptEventType::PRESS);
            if (currentState != lastStableState)
            {
                lastDebounceTime = now;
            }
            if ((now - lastDebounceTime) * portTICK_PERIOD_MS >= debounceDelayMs)
            {
                if (currentState != debouncedState)
                {
                    debouncedState = currentState;
                    lastStableState = currentState;
                    if (debouncedState)
                    {
                        // Debouncing finished: switch pressed
                        pressStartTime_ = xTaskGetTickCount();
                    }
                    else
                    {
                        // Debouncing finished: switch released
                        TickType_t now = xTaskGetTickCount();
                        TickType_t elapsedMs = (now - pressStartTime_) * portTICK_PERIOD_MS;
                        if (elapsedMs >= longPressThresholdMs_)
                        {
                            ESP_LOGI(pcTaskGetName(nullptr), "Long press detected: %lu ms", elapsedMs);
                            bool legal = HandleLongPress();
                            if (!legal)
                            {
                                ESP_LOGW(pcTaskGetName(nullptr), "Long press not legal in current state");
                            }
                        }
                        if (HandleShortPress() != ESP_OK)
                        {
                            ESP_LOGW(pcTaskGetName(nullptr), "Short press not legal in current state");
                        }
                    }
                }
            }
        }
        else if (xQueueReceive(configEventQueue, &configEvent, 0) == pdTRUE)
        {
            if (configEvent.type == Messages::EventType::UPDATE_CONFIGURATION)
            {

                configuration_.lock();
                polarityNormallyOpen_ = configuration_.getFootSwitchPolarityNormallyOpen();
                longPressThresholdMs_ = configuration_.getFootSwitchLongPressTime();
                configuration_.unlock();
                ESP_LOGI(pcTaskGetName(nullptr),
                    "Configuration updated: polarityNormallyOpen=%d, longPressThresholdMs=%d", polarityNormallyOpen_,
                    longPressThresholdMs_);
            }
            else
            {
                ESP_LOGW(pcTaskGetName(nullptr), "Unknown event type received in FootSwitch: %d", configEvent.type);
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

esp_err_t FootSwitch::HandleShortPress()
{
    ESP_LOGI(pcTaskGetName(nullptr), "Short press detected");

    // send next preset event to DMX Controller
    Messages::Event event;
    event.type = Messages::USER_NEXT_PRESET;

    if (xQueueSend(getDmxControllerEventQueue(), &event, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "Failed to send USER_NEXT_PRESET event to DMX Controller");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t FootSwitch::HandleLongPress()
{
    ESP_LOGI(pcTaskGetName(nullptr), "Long press detected");
    // send previous preset event to DMX Controller
    Messages::Event event;
    event.type = Messages::USER_PREVIOUS_PRESET;
    if (xQueueSend(getDmxControllerEventQueue(), &event, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(pcTaskGetName(nullptr), "Failed to send USER_PREVIOUS_PRESET event to DMX Controller");
        return ESP_FAIL;
    }
    return ESP_OK;
}
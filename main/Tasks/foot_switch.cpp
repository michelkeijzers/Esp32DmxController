#ifdef _MSC_VER
#ifndef portYIELD_FROM_ISR
#define portYIELD_FROM_ISR() ((void)0)
#endif
#endif
#include "foot_switch.hpp"
#include "esp_attr.h"
#include "messages.hpp"
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <stdio.h>

static const int QUEUE_CAPACITY = 10;

static QueueHandle_t interrupt_event_queue = nullptr;
static QueueHandle_t config_event_queue = nullptr;
#ifndef _MSC_VER
static QueueSetHandle_t footswitch_queue_set = nullptr;
#endif
static gpio_num_t isr_pin = GPIO_NUM_NC;

// ISR handler: minimal, just post event to queue
static void IRAM_ATTR isr_handler(void *arg)
{
    // FootSwitch *footSwitch = static_cast<FootSwitch *>(arg); // TODO: UNUSED VARIABLE
    int level = gpio_get_level(isr_pin);
    FootSwitch::InterruptEvent event;
    event.type = (level == 0) ? InterruptEventType::PRESS : InterruptEventType::RELEASE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(interrupt_event_queue, &event, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

FootSwitch::FootSwitch()
    : RtosTask(), pin_(GPIO_NUM_NC), lastPinState_(false), pressStartTime_(0),
      longPressTimeMs_(1000), // Default long press time
      polarityNormallyOpen_(true), longPressThresholdMs_(1000)
{
}

FootSwitch::~FootSwitch() {}

esp_err_t FootSwitch::init(RtosTask::TaskProperties taskProperties, gpio_num_t pinNum)
{
    if (RtosTask::init(taskProperties) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize FootSwitchTask");
        return ESP_FAIL;
    }

    // Classic C++ initialization for unit test build
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << pinNum);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_ANYEDGE; // Enable interrupt on both edges

    if (gpio_config(&io_conf) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to configure GPIO pin %d", pinNum);
        return ESP_FAIL;
    }
    // Read initial state of the pin and set lastPinState accordingly
    int initialLevel = gpio_get_level(pinNum);
    lastPinState_ = (initialLevel == 0);

    pin_ = pinNum;
    isr_pin = pinNum;
    interrupt_event_queue = xQueueCreate(QUEUE_CAPACITY, sizeof(FootSwitch::InterruptEvent));
    if (interrupt_event_queue == nullptr)
    {
        ESP_LOGE(log_tag_, "Failed to create foot switch interrupt event queue");
        return ESP_FAIL;
    }
    config_event_queue = getEventQueue();
#ifndef _MSC_VER
    footswitch_queue_set = xQueueCreateSet(QUEUE_CAPACITY * 2);
    if (footswitch_queue_set == nullptr)
    {
        ESP_LOGE(log_tag_, "Failed to create queue set");
        return ESP_FAIL;
    }
    xQueueAddToSet(interrupt_event_queue, footswitch_queue_set);
    xQueueAddToSet(config_event_queue, footswitch_queue_set);
#endif

    if (gpio_install_isr_service(0) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to install GPIO ISR service");
        return ESP_FAIL;
    }

    if (gpio_isr_handler_add(pin_, isr_handler, this) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to add GPIO ISR handler");
        return ESP_FAIL;
    }

    ESP_LOGI(log_tag_, "FootSwitchTask task started (interrupt mode)");
    return ESP_OK;
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
#ifndef _MSC_VER
    while (true)
    {
        QueueHandle_t activatedQueue = xQueueSelectFromSet(footswitch_queue_set, portMAX_DELAY);
        if (activatedQueue == interrupt_event_queue)
        {
            if (xQueueReceive(interrupt_event_queue, &interruptEvent, 0) == pdTRUE)
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
                                ESP_LOGI(log_tag_, "Long press detected: %lu ms", elapsedMs);
                                bool legal = HandleLongPress();
                                if (!legal)
                                {
                                    ESP_LOGW(log_tag_, "Long press not legal in current state");
                                }
                            }
                            if (HandleShortPress() != ESP_OK)
                            {
                                ESP_LOGW(log_tag_, "Short press not legal in current state");
                            }
                        }
                    }
                }
            }
        }
        else if (activatedQueue == config_event_queue)
        {
            if (xQueueReceive(config_event_queue, &configEvent, 0) == pdTRUE)
            {
                if (configEvent.type == Messages::EventType::SET_CONFIGURATION)
                {
                    polarityNormallyOpen_ = configEvent.data.configurationData.switchPolarityNormallyOpen;
                    longPressThresholdMs_ = configEvent.data.configurationData.longPressThresholdMs;
                    ESP_LOGI(log_tag_, "Configuration updated: polarityNormallyOpen=%d, longPressThresholdMs=%d",
                        polarityNormallyOpen_, longPressThresholdMs_);
                }
                else
                {
                    ESP_LOGW(log_tag_, "Unknown event type received in FootSwitch: %d", configEvent.type);
                }
            }
        }
    }
#else
    // For unit tests (MSVC), just block on interrupt_event_queue and poll config_event_queue
    while (true)
    {
        if (xQueueReceive(interrupt_event_queue, &interruptEvent, portMAX_DELAY) == pdTRUE)
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
                        pressStartTime_ = xTaskGetTickCount();
                    }
                    else
                    {
                        TickType_t now = xTaskGetTickCount();
                        TickType_t elapsedMs = (now - pressStartTime_) * portTICK_PERIOD_MS;
                        if (elapsedMs >= longPressThresholdMs_)
                        {
                            ESP_LOGI(log_tag_, "Long press detected: %lu ms", elapsedMs);
                            bool legal = HandleLongPress();
                            if (!legal)
                            {
                                ESP_LOGW(log_tag_, "Long press not legal in current state");
                            }
                        }
                        if (HandleShortPress() != ESP_OK)
                        {
                            ESP_LOGW(log_tag_, "Short press not legal in current state");
                        }
                    }
                }
            }
        }
        if (xQueueReceive(config_event_queue, &configEvent, 0) == pdTRUE)
        {
            if (configEvent.type == Messages::EventType::SET_CONFIGURATION)
            {
                polarityNormallyOpen_ = configEvent.data.configurationData.switchPolarityNormallyOpen;
                longPressThresholdMs_ = configEvent.data.configurationData.longPressThresholdMs;
                ESP_LOGI(log_tag_, "Configuration updated: polarityNormallyOpen=%d, longPressThresholdMs=%d",
                    polarityNormallyOpen_, longPressThresholdMs_);
            }
            else
            {
                ESP_LOGW(log_tag_, "Unknown event type received in FootSwitch: %d", configEvent.type);
            }
        }
    }
#endif
}

esp_err_t FootSwitch::HandleShortPress()
{
    ESP_LOGI(log_tag_, "Short press detected");

    // send next preset event to DMX Controller
    Messages::Event event;
    event.type = Messages::USER_NEXT_PRESET;

    if (xQueueSend(getDmxControllerEventQueue(), &event, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send USER_NEXT_PRESET event to DMX Controller");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t FootSwitch::HandleLongPress()
{
    ESP_LOGI(log_tag_, "Long press detected");
    // send previous preset event to DMX Controller
    Messages::Event event;
    event.type = Messages::USER_PREVIOUS_PRESET;
    if (xQueueSend(getDmxControllerEventQueue(), &event, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send USER_PREVIOUS_PRESET event to DMX Controller");
        return ESP_FAIL;
    }
    return ESP_OK;
}
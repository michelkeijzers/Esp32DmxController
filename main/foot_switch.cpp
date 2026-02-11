#include "foot_switch.hpp"
#include <stdio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <freertos/queue.h>

static QueueHandle_t foot_switch_interrupt_event_queue = nullptr;

// ISR handler: minimal, just post event to queue
static void IRAM_ATTR foot_switch_isr_handler(void *arg)
{
    FootSwitch *footSwitch = static_cast<FootSwitch *>(arg);
    int level = gpio_get_level(footSwitch->getPin());
    FootSwitch::FootSwitchInterruptEvent event;
    event.type = (level == 0) ? FootSwitchInterruptEventType::PRESS : FootSwitchInterruptEventType::RELEASE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(foot_switch_interrupt_event_queue, &event, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

static const char *LOG_TAG = "FootSwitch";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

FootSwitch::FootSwitch()
    : RtosTask(),
      _pin(GPIO_NUM_NC),
      lastPinState(false),
      pressStartTime(0),
      longPressTimeMs(1000), // Default long press time
      polarityInverted(false),
      longPressThresholdMs(1000),
      state_(State::BOOT)
{
}

FootSwitch::~FootSwitch()
{
}

esp_err_t FootSwitch::init(gpio_num_t pinNum)
{
    if (RtosTask::init("FootSwitchTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(FootSwitchEvent)) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize FootSwitchTask");
        return ESP_FAIL;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pinNum),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE}; // Enable interrupt on both edges

    if (gpio_config(&io_conf) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to configure GPIO pin %d", pinNum);
        return ESP_FAIL;
    }
    // Read initial state of the pin and set lastPinState accordingly
    int initialLevel = gpio_get_level(pinNum);
    lastPinState = (initialLevel == 0);
    state_ = lastPinState ? State::OTA_CHECK : State::NORMAL_OPERATION;

    _pin = pinNum;
    foot_switch_interrupt_event_queue = xQueueCreate(QUEUE_CAPACITY, sizeof(FootSwitch::FootSwitchInterruptEvent));
    if (foot_switch_interrupt_event_queue == nullptr)
    {
        ESP_LOGE(LOG_TAG, "Failed to create foot switch interrupt event queue");
        return ESP_FAIL;
    }

    if (gpio_install_isr_service(0) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to install GPIO ISR service");
        return ESP_FAIL;
    }

    if (gpio_isr_handler_add(_pin, foot_switch_isr_handler, this) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to add GPIO ISR handler");
        return ESP_FAIL;
    }

    ESP_LOGI(LOG_TAG, "FootSwitchTask task started (interrupt mode)");
    return ESP_OK;
}

void FootSwitch::taskEntry(void *param)
{
    static_cast<FootSwitch *>(param)->taskLoop();
}

void FootSwitch::taskLoop()
{
    FootSwitchInterruptEvent event;
    const int debounceDelayMs = 30; // Debounce delay in ms
    bool debouncedState = false;
    bool lastStableState = false;
    TickType_t lastDebounceTime = 0;
    while (true)
    {
        if (xQueueReceive(foot_switch_interrupt_event_queue, &event, portMAX_DELAY) == pdTRUE)
        {
            TickType_t now = xTaskGetTickCount();
            bool currentState = (event.type == FootSwitchInterruptEventType::PRESS);
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
                        pressStartTime = xTaskGetTickCount();
                    }
                    else
                    {
                        // Debouncing finished: switch released
                        TickType_t now = xTaskGetTickCount();
                        TickType_t elapsedMs = (now - pressStartTime) * portTICK_PERIOD_MS;
                        if (elapsedMs >= longPressThresholdMs)
                        {
                            ESP_LOGI(LOG_TAG, "Long press detected: %lu ms", elapsedMs);
                            bool legal = HandleLongPress();
                            if (!legal)
                            {
                                ESP_LOGW(LOG_TAG, "Long press not legal in current state");
                            }
                        }
                        bool legal = HandleShortPress();
                        if (!legal)
                        {
                            ESP_LOGW(LOG_TAG, "Short press not legal in current state");
                        }
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool FootSwitch::HandleShortPress()
{
    ESP_LOGI(LOG_TAG, "Short press detected");
    switch (state_)
    {
    case State::BOOT:
        // Illegal
        return false;
        break;

    case State::OTA_CHECK:
        // TODO: Next Preset
        state_ = State::NORMAL_OPERATION;
        break;

    case State::OTA:
        // Ignore
        break;

    case State::NORMAL_OPERATION:
        // TODO: Next Preset, no state change.
        break;

    default:
        return false;
    }
    return true;
}

bool FootSwitch::HandleLongPress()
{
    ESP_LOGI(LOG_TAG, "Long press detected");
    switch (state_)
    {
    case State::BOOT:
        // Illegal
        return false;
        break;

    case State::OTA_CHECK:
        // TODO: OTA Update
        state_ = State::OTA;
        break;

    case State::OTA:
        // Ignore
        break;

    case State::NORMAL_OPERATION:
        // TODO: Previous Preset, no state change.
        break;

    default:
        return false;
    }
    return true;
}
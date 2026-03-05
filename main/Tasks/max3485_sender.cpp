#include "max3485_sender.hpp"
#include <cstring>
#ifdef ESP_PLATFORM
#include "esp_rom_sys.h"
#include <driver/uart.h>
#include <esp_log.h>
#else
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#endif
#include "../Data/dmx_preset.hpp"
#include <messages.hpp>

static const uart_port_t DMX_UART_PORT = UART_NUM_1;
static const int DMX_TX_PIN = 17; // Set to your TX pin
static const int DMX_BAUD_RATE = 250000;
static const int DMX_BREAK_US = 100;
static const int DMX_MAB_US = 12;

Max3485Sender::Max3485Sender() : RtosTask() {}

Max3485Sender::~Max3485Sender() { close(); }

esp_err_t Max3485Sender::init(RtosTask::TaskProperties taskProperties)
{
    RtosTask::init(taskProperties);

    uart_config_t uart_config;
    uart_config.baud_rate = DMX_BAUD_RATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_2;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    ESP_ERROR_CHECK(uart_param_config(DMX_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(
        uart_set_pin(DMX_UART_PORT, DMX_TX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(DMX_UART_PORT, 1024, 0, 0, NULL, 0));
    initialized_ = true;
    ESP_LOGI(logTag_, "MAX3485 sender initialized");
    return ESP_OK;
}

void Max3485Sender::close()
{
    if (initialized_)
    {
        uart_driver_delete(DMX_UART_PORT);
        initialized_ = false;
    }
}

esp_err_t Max3485Sender::sendDmx(const uint8_t *data, uint16_t length)
{
    if (!initialized_)
    {
        ESP_LOGE(logTag_, "MAX3485 sender not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    if (!data || length == 0 || length > 512)
    {
        ESP_LOGE(logTag_, "Invalid DMX data or length");
        return ESP_ERR_INVALID_ARG;
    }
    // DMX break
    uart_wait_tx_done(DMX_UART_PORT, portMAX_DELAY);
    esp_rom_delay_us(DMX_BREAK_US);
    // Mark After Break
    esp_rom_delay_us(DMX_MAB_US);
    // Send DMX data
    int written = uart_write_bytes(DMX_UART_PORT, (const char *)data, length);
    if (written != length)
    {
        ESP_LOGE(logTag_, "Failed to write all DMX bytes");
        return ESP_FAIL;
    }
    ESP_LOGD(logTag_, "Sent DMX frame (%d bytes)", length);
    return ESP_OK;
}

void Max3485Sender::taskEntry(void *param) { static_cast<Max3485Sender *>(param)->taskLoop(); }

void Max3485Sender::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event.type)
            {
            case Messages::SEND_PRESET_DATA:
                // TODO sendDmx(event.data.presetData.dmxValues, NR_OF_DMX_CHANNELS);
                //  Optionally send response event
                break;
            default:
                break;
            }
        }
    }
}

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
#include "../Base/assert.hpp"
#include "../Data/dmx_preset.hpp"
#include <messages.hpp>

static const uart_port_t DMX_UART_PORT = UART_NUM_1;
static const int DMX_TX_PIN = 17; // Set to your TX pin
static const int DMX_BAUD_RATE = 250000;
static const int DMX_BREAK_US = 100;
static const int DMX_MAB_US = 12;

Max3485Sender::Max3485Sender(IAssert *assert) : RtosTask(), assert_(assert) {}

Max3485Sender::~Max3485Sender() { close(); }

void Max3485Sender::init(RtosTask::TaskProperties taskProperties)
{
    RtosTask::init(taskProperties);

    uart_config_t uart_config;
    uart_config.baud_rate = DMX_BAUD_RATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_2;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    assert_->assertNotEspError(uart_param_config(DMX_UART_PORT, &uart_config), "Failed to configure UART");
    assert_->assertNotEspError(
        uart_set_pin(DMX_UART_PORT, DMX_TX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE),
        "Failed to set UART pins");
    assert_->assertNotEspError(
        uart_driver_install(DMX_UART_PORT, 1024, 0, 0, NULL, 0), "Failed to install UART driver");
    initialized_ = true;
    ESP_LOGI(pcTaskGetName(nullptr), "MAX3485 sender initialized");
}

void Max3485Sender::close()
{
    if (initialized_)
    {
        uart_driver_delete(DMX_UART_PORT);
        initialized_ = false;
    }
}

void Max3485Sender::sendDmx(const uint8_t *data, uint16_t length)
{
    assert_->assertTrue(initialized_, "MAX3485 sender not initialized");
    assert_->assertNotNull(data, "DMX data pointer is null");
    assert_->assertTrue(length <= 512, "DMX data length must be between 0 and 512");

    // DMX break
    uart_wait_tx_done(DMX_UART_PORT, portMAX_DELAY);
    esp_rom_delay_us(DMX_BREAK_US);
    // Mark After Break
    esp_rom_delay_us(DMX_MAB_US);
    // Send DMX data
    int written = uart_write_bytes(DMX_UART_PORT, (const char *)data, length);
    assert_->assertTrue(written == length, "Failed to write all DMX bytes");
    ESP_LOGD(pcTaskGetName(nullptr), "Sent DMX frame (%d bytes)", length);
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

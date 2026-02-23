#pragma once

#include <esp_err.h>
#include <stdint.h>
extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "Base/rtos_task.hpp"

// Class to send DMX data to MAX3485 board
class Max3485Sender : public RtosTask
{
  public:
    Max3485Sender();
    ~Max3485Sender();

    virtual esp_err_t init(QueueHandle_t dmxControllerEventQueue);
    void close();

    esp_err_t sendDmx(const uint8_t *data, uint16_t length);

  private:
    void taskEntry(void *param) override;
    void taskLoop();
    // Add members for UART handle, etc.
    bool initialized_;
};

#pragma once

#include <stdint.h>
extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "Base/rtos_task.hpp"

class IAssert;
// Class to send DMX data to MAX3485 board

class Max3485Sender : public RtosTask
{
  public:
    Max3485Sender(IAssert *assert);
    ~Max3485Sender();

    virtual void init(RtosTask::TaskProperties taskProperties);

  protected:
    int taskPriority_;
    int queueCapacity_;
    void close();

    void sendDmx(const uint8_t *data, uint16_t length);

    IAssert *assert_;

  private:
    void taskEntry(void *param) override;
    void taskLoop();
    // Add members for UART handle, etc.
    bool initialized_;
};

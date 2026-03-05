#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

class Lockable
{
  private:
    SemaphoreHandle_t mutex_;

  protected:
    Lockable() { mutex_ = xSemaphoreCreateMutex(); }
    ~Lockable() { vSemaphoreDelete(mutex_); }

  public:
    void lock() { xSemaphoreTake(mutex_, portMAX_DELAY); } // TODO: Check PdPass
    void unlock() { xSemaphoreGive(mutex_); }
    bool tryLock(TickType_t timeout = 0) { return xSemaphoreTake(mutex_, timeout) == pdTRUE; }
};
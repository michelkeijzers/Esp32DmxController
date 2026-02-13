#pragma once

// 7-Segment Display Controller Class
// Controls a single digit 7-segment display with decimal point
// 8 segments total: A, B, C, D, E, F, G, DP

#include "rtos_task.hpp"
#include <driver/gpio.h>
#include <esp_err.h>
#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

class SevenSegmentDisplay : public RtosTask {
  public:
    struct Event {
        char character;
        bool dot;
    };

    SevenSegmentDisplay();
    ~SevenSegmentDisplay();

    esp_err_t init(QueueHandle_t dmxControllerEventQueue, const gpio_num_t pins[8]);

  private:
    enum Segment { SEG_A = 0, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP };
    gpio_num_t segmentPins_[8];
    uint8_t currentPattern_;
    bool decimalPointOn_;

    esp_err_t updateDisplay();

    esp_err_t displayDigit(char character, bool dot);
    esp_err_t setSegment(Segment segment, bool on);
    esp_err_t setDecimalPoint(bool on);

    ///  --- a ---
    ///  |       |
    ///  f       b
    ///  |       |
    ///  --- g ---
    ///  |       |
    ///  e       c
    ///  |       |
    ///  --- d --- dp

    // Segment encoding: g f e d c b a
    // Example: 0b0111111 = digit "0" (a,b,c,d,e,f ON, g OFF)

    static const uint8_t digitPatterns_[];
    void taskEntry(void *param) override;
    void taskLoop();
};
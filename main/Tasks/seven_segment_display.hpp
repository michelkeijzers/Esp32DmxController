#pragma once

// 7-Segment Display Controller Class
// Controls a single digit 7-segment display with decimal point
// 8 segments total: A, B, C, D, E, F, G, DP

#include "Base/rtos_task.hpp"
#include <driver/gpio.h>
#include <esp_err.h>
#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

class IAssert;

class SevenSegmentDisplay : public RtosTask
{
    friend class SevenSegmentDisplayTest_Friend;

  public:
    struct Event
    {
        char character;
        bool dot;
    };

    SevenSegmentDisplay(IAssert *assert);
    ~SevenSegmentDisplay();

    virtual void init(RtosTask::TaskProperties taskProperties, const gpio_num_t pins[8]);

  private:
    IAssert *assert_;

  protected:
    int taskPriority_;
    int queueCapacity_;

    enum Segment
    {
        SEG_A = 0,
        SEG_B,
        SEG_C,
        SEG_D,
        SEG_E,
        SEG_F,
        SEG_G,
        SEG_DP
    };
    gpio_num_t segmentPins_[8];
    uint8_t currentPattern_;
    bool decimalPointOn_;

    void updateDisplay();

    void displayDigit(char character, bool dot);
    void setSegment(Segment segment, bool on);
    void setDecimalPoint(bool on);

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
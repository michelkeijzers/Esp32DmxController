#pragma once
#include "../../main/Tasks/seven_segment_display.hpp"
#include <gmock/gmock.h>
class MockSevenSegmentDisplay : public SevenSegmentDisplay
{
  public:
    MockSevenSegmentDisplay(IAssert* assert) : SevenSegmentDisplay(assert) {}
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, const gpio_num_t *));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

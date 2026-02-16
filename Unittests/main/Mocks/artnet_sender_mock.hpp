#pragma once
#include "../../main/artnet_sender.hpp"
#include <gmock/gmock.h>
class MockArtNetSender : public ArtNetSender
{
  public:
    MOCK_METHOD(esp_err_t, init, (QueueHandle_t, const char *, uint16_t));
    MOCK_METHOD(QueueHandle_t, getEventQueue, ());
};

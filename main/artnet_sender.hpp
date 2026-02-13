#pragma once

#include <esp_err.h>
#include <lwip/sockets.h>
#include <stdint.h>

// Art-Net protocol implementation for ESP32
// Sends DMX data over UDP to Art-Net receivers

#define ARTNET_PORT 6454
#define ARTNET_MAX_UNIVERSES 2

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
}
#include "rtos_task.hpp"

class ArtNetSender : public RtosTask
{
  public:
    struct ArtNetHeader
    {
        char id[8];
        uint16_t opcode;
        uint16_t version;
    };

    struct ArtNetDmxPacket
    {
        ArtNetHeader header;
        uint8_t sequence;
        uint8_t physical;
        uint16_t universe;
        uint16_t length;
        uint8_t data[512];
    };

    ArtNetSender();
    ~ArtNetSender();

    esp_err_t init(QueueHandle_t dmxControllerEventQueue, const char *dest_ip, uint16_t dest_port = ARTNET_PORT);

    void close();

    esp_err_t sendUniverse(uint16_t universe, const uint8_t *data, uint16_t length);
    esp_err_t sendUniverses(
        const uint8_t *universe_1_data, uint16_t len_1, const uint8_t *universe_2_data, uint16_t len_2);

  private:
    int sockfd_;
    struct sockaddr_in dest_addr_;
    uint8_t sequence_counter_;

    void taskEntry(void *param) override;
    void taskLoop();

    void createDmxPacket(ArtNetDmxPacket &packet, uint16_t universe, const uint8_t *data, uint16_t length);
};
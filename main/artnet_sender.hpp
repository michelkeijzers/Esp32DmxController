#pragma once

#include <stdint.h>
#include <esp_err.h>
#include <lwip/sockets.h>

// Art-Net protocol implementation for ESP32
// Sends DMX data over UDP to Art-Net receivers

#define ARTNET_PORT 6454
#define ARTNET_MAX_UNIVERSES 2

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
}

class ArtNetSender
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

    enum EventType
    {
        SEND_UNIVERSE,
        SEND_UNIVERSES
    };

    struct SendEvent
    {
        EventType type;
        union
        {
            struct
            {
                uint16_t universe;
                const uint8_t *data;
                uint16_t length;
            } single;
            struct
            {
                const uint8_t *universe1_data;
                uint16_t len1;
                const uint8_t *universe2_data;
                uint16_t len2;
            } multi;
        } payload;
    };

    ArtNetSender();
    ~ArtNetSender();

    esp_err_t init(const char *dest_ip, uint16_t dest_port = ARTNET_PORT);

    // Post a send event to the task
    void postSendEvent(const SendEvent &event);

    void close();

    esp_err_t sendUniverses(const uint8_t *universe1_data, uint16_t len1,
                            const uint8_t *universe2_data, uint16_t len2);
    esp_err_t sendUniverse(uint16_t universe, const uint8_t *data, uint16_t length);

private:
    int sockfd_;
    struct sockaddr_in dest_addr_;
    bool initialized_;
    uint8_t sequence_counter_;

    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;

    static void taskEntry(void *param);
    void taskLoop();

    void createDmxPacket(ArtNetDmxPacket &packet, uint16_t universe,
                         const uint8_t *data, uint16_t length);
};
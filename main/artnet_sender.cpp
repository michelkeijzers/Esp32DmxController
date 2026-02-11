#include "artnet_sender.hpp"
#include <esp_log.h>
#include <cstring>
#include <lwip/inet.h>

static const char *TAG = "ArtNetSender";

ArtNetSender::ArtNetSender()
    : sockfd_(-1), initialized_(false), sequence_counter_(0),
      taskHandle_(nullptr), eventQueue_(nullptr)
{
    memset(&dest_addr_, 0, sizeof(dest_addr_));
    eventQueue_ = xQueueCreate(4, sizeof(SendEvent));
    if (eventQueue_)
    {
        xTaskCreate(taskEntry, "ArtNetSenderTask", 4096, this, 5, &taskHandle_);
        ESP_LOGI(TAG, "ArtNetSender task started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create ArtNetSender event queue");
    }
}

ArtNetSender::~ArtNetSender()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    if (eventQueue_)
    {
        vQueueDelete(eventQueue_);
    }
}

void ArtNetSender::postSendEvent(const SendEvent &event)
{
    if (eventQueue_)
    {
        xQueueSend(eventQueue_, &event, 0);
    }
}

void ArtNetSender::taskEntry(void *param)
{
    static_cast<ArtNetSender *>(param)->taskLoop();
}

void ArtNetSender::taskLoop()
{
    SendEvent event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event.type)
            {
            case SEND_UNIVERSE:
                sendUniverse(event.payload.single.universe, event.payload.single.data, event.payload.single.length);
                break;
            case SEND_UNIVERSES:
                sendUniverses(event.payload.multi.universe1_data, event.payload.multi.len1,
                              event.payload.multi.universe2_data, event.payload.multi.len2);
                break;
            }
        }
    }
}

esp_err_t ArtNetSender::init(const char *dest_ip, uint16_t dest_port)
{
    if (!dest_ip)
    {
        ESP_LOGE(TAG, "Invalid destination IP");
        return ESP_ERR_INVALID_ARG;
    }

    // Create UDP socket
    sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd_ < 0)
    {
        ESP_LOGE(TAG, "Failed to create socket");
        return ESP_FAIL;
    }

    // Set up destination address
    dest_addr_.sin_family = AF_INET;
    dest_addr_.sin_port = htons(dest_port);
    if (inet_pton(AF_INET, dest_ip, &dest_addr_.sin_addr) != 1)
    {
        ESP_LOGE(TAG, "Invalid destination IP address: %s", dest_ip);
        close();
        return ESP_ERR_INVALID_ARG;
    }

    // Set socket options for broadcast if needed
    int broadcast = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
        ESP_LOGW(TAG, "Failed to set broadcast option");
    }

    initialized_ = true;
    ESP_LOGI(TAG, "Art-Net sender initialized, destination: %s:%d", dest_ip, dest_port);
    return ESP_OK;
}

esp_err_t ArtNetSender::sendUniverse(uint16_t universe, const uint8_t *data, uint16_t length)
{
    if (!initialized_)
    {
        ESP_LOGE(TAG, "Art-Net sender not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!data || length == 0 || length > 512)
    {
        ESP_LOGE(TAG, "Invalid data or length");
        return ESP_ERR_INVALID_ARG;
    }

    // Ensure length is even (Art-Net requirement)
    if (length % 2 != 0)
    {
        length++; // Pad to even length
    }

    ArtNetDmxPacket packet;
    createDmxPacket(packet, universe, data, length);

    // Send packet
    ssize_t sent = sendto(sockfd_, &packet, sizeof(ArtNetHeader) + 4 + length,
                          0, (struct sockaddr *)&dest_addr_, sizeof(dest_addr_));

    if (sent < 0)
    {
        ESP_LOGE(TAG, "Failed to send Art-Net packet");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Sent Art-Net universe %d (%d bytes)", universe, length);
    return ESP_OK;
}

esp_err_t ArtNetSender::sendUniverses(const uint8_t *universe1_data, uint16_t len1,
                                      const uint8_t *universe2_data, uint16_t len2)
{
    esp_err_t ret;

    // Send universe 1
    if (universe1_data && len1 > 0)
    {
        ret = sendUniverse(0, universe1_data, len1);
        if (ret != ESP_OK)
        {
            return ret;
        }
    }

    // Send universe 2
    if (universe2_data && len2 > 0)
    {
        ret = sendUniverse(1, universe2_data, len2);
        if (ret != ESP_OK)
        {
            return ret;
        }
    }

    return ESP_OK;
}

void ArtNetSender::close()
{
    if (sockfd_ >= 0)
    {
        ::close(sockfd_);
        sockfd_ = -1;
    }
    initialized_ = false;
}

void ArtNetSender::createDmxPacket(ArtNetDmxPacket &packet, uint16_t universe,
                                   const uint8_t *data, uint16_t length)
{
    // Set header
    memcpy(packet.header.id, "Art-Net\0", 8);
    packet.header.opcode = 0x0050; // OpOutput (little endian)
    packet.header.version = 14;    // Protocol version

    // Set DMX data
    packet.sequence = sequence_counter_++;
    packet.physical = 0; // Physical port
    packet.universe = universe;
    packet.length = length;

    // Copy DMX data
    memcpy(packet.data, data, length);

    // Pad with zeros if needed
    if (length < 512)
    {
        memset(packet.data + length, 0, 512 - length);
    }
}
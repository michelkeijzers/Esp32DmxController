#include "artnet_sender.hpp"
#include <cstring>
#include <esp_log.h>
#include <lwip/inet.h>
#include <messages.hpp>

static const char *LOG_TAG = "ArtNetSender";
static const int QUEUE_CAPACITY = 20;
static const int TASK_PRIORITY = 5;

ArtNetSender::ArtNetSender() : RtosTask(), sockfd_(-1), sequence_counter_(0)
{
    memset(&dest_addr_, 0, sizeof(dest_addr_));
}

ArtNetSender::~ArtNetSender() {}

void ArtNetSender::taskEntry(void *param) { static_cast<ArtNetSender *>(param)->taskLoop(); }

esp_err_t ArtNetSender::init(QueueHandle_t dmxControllerEventQueue, const char *dest_ip, uint16_t dest_port)
{
    if (RtosTask::init("ArtNetSenderTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Messages::Event),
            dmxControllerEventQueue) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize ArtNetSenderTask");
        return ESP_FAIL;
    }

    if (!dest_ip)
    {
        ESP_LOGE(LOG_TAG, "Invalid destination IP");
        return ESP_ERR_INVALID_ARG;
    }

    // Create UDP socket
    sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd_ < 0)
    {
        ESP_LOGE(LOG_TAG, "Failed to create socket");
        return ESP_FAIL;
    }

    // Set up destination address
    dest_addr_.sin_family = AF_INET;
    dest_addr_.sin_port = htons(dest_port);
    if (inet_pton(AF_INET, dest_ip, &dest_addr_.sin_addr) != 1)
    {
        ESP_LOGE(LOG_TAG, "Invalid destination IP address: %s", dest_ip);
        close();
        return ESP_ERR_INVALID_ARG;
    }

    // Set socket options for broadcast if needed
    int broadcast = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
        ESP_LOGW(LOG_TAG, "Failed to set broadcast option");
    }

    initialized_ = true;
    ESP_LOGI(LOG_TAG, "Art-Net sender initialized, destination: %s:%d", dest_ip, dest_port);
    return ESP_OK;
}

void ArtNetSender::taskLoop()
{
    Messages::Event event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event.type)
            {
            case Messages::SEND_PRESET_DATA:
                sendUniverses(event.data.presetData.universe1Data, event.data.presetData.universe1Length,
                    event.data.presetData.universe2Data, event.data.presetData.universe2Length);

                // Send response back to DmxController (no response needed, but can be used for logging)
                Messages::Event responseEvent;
                responseEvent.type = Messages::EventType::SEND_PRESET_DATA_RESPONSE;
                // TODO: Fill ack/nack
                if (xQueueSend(getDmxControllerEventQueue(), &responseEvent, 0) != pdPASS)
                {
                    ESP_LOGE(LOG_TAG, "Failed to send preset data sent response to DmxController");
                }
                break;

            default:
                // Ignore others.
                break;
            }
        }
    }
}

esp_err_t ArtNetSender::sendUniverse(uint16_t universe, const uint8_t *data, uint16_t length)
{
    if (!initialized_)
    {
        ESP_LOGE(LOG_TAG, "Art-Net sender not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!data || length == 0 || length > 512)
    {
        ESP_LOGE(LOG_TAG, "Invalid data or length");
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
    ssize_t sent = sendto(
        sockfd_, &packet, sizeof(ArtNetHeader) + 4 + length, 0, (struct sockaddr *)&dest_addr_, sizeof(dest_addr_));

    if (sent < 0)
    {
        ESP_LOGE(LOG_TAG, "Failed to send Art-Net packet");
        return ESP_FAIL;
    }

    ESP_LOGD(LOG_TAG, "Sent Art-Net universe %d (%d bytes)", universe, length);
    return ESP_OK;
}

esp_err_t ArtNetSender::sendUniverses(
    const uint8_t *universe1_data, uint16_t len1, const uint8_t *universe2_data, uint16_t len2)
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

void ArtNetSender::createDmxPacket(ArtNetDmxPacket &packet, uint16_t universe, const uint8_t *data, uint16_t length)
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
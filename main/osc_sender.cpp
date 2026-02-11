#include "osc_sender.hpp"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <esp_log.h>
#include <cstring>
#include <algorithm>

static const char *TAG = "OSC";

OSCSender::OSCSender() : sockfd(-1), initialized(false)
{
}

OSCSender::~OSCSender()
{
    close();
}

esp_err_t OSCSender::init(const char *dest_ip, uint16_t dest_port)
{
    if (initialized)
    {
        close();
    }

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
    {
        ESP_LOGE(TAG, "Failed to create socket");
        return ESP_FAIL;
    }

    // Set up destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);

    if (inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr) != 1)
    {
        ESP_LOGE(TAG, "Invalid destination IP address");
        close();
        return ESP_FAIL;
    }

    initialized = true;
    ESP_LOGI(TAG, "OSC sender initialized to %s:%d", dest_ip, dest_port);
    return ESP_OK;
}

void OSCSender::writeInt32(std::vector<uint8_t> &buffer, int32_t value)
{
    // OSC uses big-endian byte order
    buffer.push_back((value >> 24) & 0xFF);
    buffer.push_back((value >> 16) & 0xFF);
    buffer.push_back((value >> 8) & 0xFF);
    buffer.push_back(value & 0xFF);
}

void OSCSender::writeFloat(std::vector<uint8_t> &buffer, float value)
{
    // Convert float to big-endian int32 representation
    uint32_t int_value;
    memcpy(&int_value, &value, sizeof(float));

    // Swap to big-endian
    int_value = ((int_value >> 24) & 0xFF) |
                ((int_value >> 8) & 0xFF00) |
                ((int_value << 8) & 0xFF0000) |
                ((int_value << 24) & 0xFF000000);

    buffer.push_back((int_value >> 24) & 0xFF);
    buffer.push_back((int_value >> 16) & 0xFF);
    buffer.push_back((int_value >> 8) & 0xFF);
    buffer.push_back(int_value & 0xFF);
}

void OSCSender::writeString(std::vector<uint8_t> &buffer, const char *str)
{
    size_t len = strlen(str);
    buffer.insert(buffer.end(), str, str + len);
    padTo4Bytes(buffer);
}

void OSCSender::padTo4Bytes(std::vector<uint8_t> &buffer)
{
    while (buffer.size() % 4 != 0)
    {
        buffer.push_back(0);
    }
}

esp_err_t OSCSender::sendMessage(const char *address, int32_t value)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "OSC sender not initialized");
        return ESP_FAIL;
    }

    std::vector<uint8_t> buffer;

    // Write OSC address
    writeString(buffer, address);

    // Write type tag string (single integer)
    writeString(buffer, ",i");

    // Write the integer value
    writeInt32(buffer, value);

    // Send the packet
    ssize_t sent = sendto(sockfd, buffer.data(), buffer.size(), 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent < 0)
    {
        ESP_LOGE(TAG, "Failed to send OSC message");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Sent OSC message: %s %ld", address, (long)value);
    return ESP_OK;
}

esp_err_t OSCSender::sendMessage(const char *address, float value)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "OSC sender not initialized");
        return ESP_FAIL;
    }

    std::vector<uint8_t> buffer;

    // Write OSC address
    writeString(buffer, address);

    // Write type tag string (single float)
    writeString(buffer, ",f");

    // Write the float value
    writeFloat(buffer, value);

    // Send the packet
    ssize_t sent = sendto(sockfd, buffer.data(), buffer.size(), 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent < 0)
    {
        ESP_LOGE(TAG, "Failed to send OSC message");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Sent OSC message: %s %f", address, value);
    return ESP_OK;
}

esp_err_t OSCSender::sendMessage(const char *address, const char *value)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "OSC sender not initialized");
        return ESP_FAIL;
    }

    std::vector<uint8_t> buffer;

    // Write OSC address
    writeString(buffer, address);

    // Write type tag string (single string)
    writeString(buffer, ",s");

    // Write the string value
    writeString(buffer, value);

    // Send the packet
    ssize_t sent = sendto(sockfd, buffer.data(), buffer.size(), 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent < 0)
    {
        ESP_LOGE(TAG, "Failed to send OSC message");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Sent OSC message: %s %s", address, value);
    return ESP_OK;
}

esp_err_t OSCSender::sendMessage(const char *address, const std::vector<int32_t> &values)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "OSC sender not initialized");
        return ESP_FAIL;
    }

    std::vector<uint8_t> buffer;

    // Write OSC address
    writeString(buffer, address);

    // Build type tag string
    std::string type_tag = ",";
    for (size_t i = 0; i < values.size(); ++i)
    {
        type_tag += "i";
    }
    writeString(buffer, type_tag.c_str());

    // Write the integer values
    for (int32_t value : values)
    {
        writeInt32(buffer, value);
    }

    // Send the packet
    ssize_t sent = sendto(sockfd, buffer.data(), buffer.size(), 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent < 0)
    {
        ESP_LOGE(TAG, "Failed to send OSC message");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Sent OSC message: %s with %d integers", address, values.size());
    return ESP_OK;
}

esp_err_t OSCSender::sendMessage(const char *address, const std::vector<float> &values)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "OSC sender not initialized");
        return ESP_FAIL;
    }

    std::vector<uint8_t> buffer;

    // Write OSC address
    writeString(buffer, address);

    // Build type tag string
    std::string type_tag = ",";
    for (size_t i = 0; i < values.size(); ++i)
    {
        type_tag += "f";
    }
    writeString(buffer, type_tag.c_str());

    // Write the float values
    for (float value : values)
    {
        writeFloat(buffer, value);
    }

    // Send the packet
    ssize_t sent = sendto(sockfd, buffer.data(), buffer.size(), 0,
                          (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent < 0)
    {
        ESP_LOGE(TAG, "Failed to send OSC message");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Sent OSC message: %s with %d floats", address, values.size());
    return ESP_OK;
}

void OSCSender::close()
{
    if (sockfd >= 0)
    {
        closesocket(sockfd);
        sockfd = -1;
    }
    initialized = false;
}
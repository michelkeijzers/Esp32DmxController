#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <esp_err.h>
#include <lwip/sockets.h>

// OSC (Open Sound Control) message implementation for ESP32
// Sends OSC messages over UDP

class OSCSender
{
private:
    int sockfd;
    struct sockaddr_in dest_addr;
    bool initialized;

    // OSC message building helpers
    void writeInt32(std::vector<uint8_t> &buffer, int32_t value);
    void writeFloat(std::vector<uint8_t> &buffer, float value);
    void writeString(std::vector<uint8_t> &buffer, const char *str);
    void padTo4Bytes(std::vector<uint8_t> &buffer);

public:
    OSCSender();
    ~OSCSender();

    // Initialize OSC sender with destination IP and port
    esp_err_t init(const char *dest_ip, uint16_t dest_port);

    // Send OSC message with address and single integer value
    esp_err_t sendMessage(const char *address, int32_t value);

    // Send OSC message with address and single float value
    esp_err_t sendMessage(const char *address, float value);

    // Send OSC message with address and string value
    esp_err_t sendMessage(const char *address, const char *value);

    // Send OSC message with address and multiple integer values
    esp_err_t sendMessage(const char *address, const std::vector<int32_t> &values);

    // Send OSC message with address and multiple float values
    esp_err_t sendMessage(const char *address, const std::vector<float> &values);

    // Close the OSC sender
    void close();
};
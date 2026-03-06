#pragma once

#include "Base/rtos_task.hpp"
#include <esp_err.h>
#include <esp_log.h>
#include <lwip/sockets.h>
#include <stdint.h>
#include <string>
#include <vector>

// OSC (Open Sound Control) message implementation for ESP32
// Sends OSC messages over UDP

class OSCSender : public RtosTask
{
  private:
    struct sockaddr_in dest_addr;
    bool initialized;

    // OSC message building helpers
    void writeInt32(std::vector<uint8_t> &buffer, int32_t value);
    void writeFloat(std::vector<uint8_t> &buffer, float value);
    void writeString(std::vector<uint8_t> &buffer, const char *str);
    void padTo4Bytes(std::vector<uint8_t> &buffer);

  public:
    OSCSender();
    virtual ~OSCSender();

    // Initialize OSC sender with destination IP and port
    virtual void init(RtosTask::TaskProperties taskProperties, const char *dest_ip, uint16_t dest_port);

  private:
    int sockfd;

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

    void taskEntry(void *param) override;
    void taskLoop();
};
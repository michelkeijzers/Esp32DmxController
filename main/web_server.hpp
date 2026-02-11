#pragma once
#include <esp_http_server.h>
#include <esp_err.h>
#include "dmx_presets.hpp"
#include <string>
#include "foot_switch.hpp"

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
}

class WebServer
{
public:
    enum EventType
    {
        START_SERVER,
        STOP_SERVER,
        RESTART_SERVER
    };
    struct WebServerEvent
    {
        EventType type;
    };

    WebServer(DmxPresets *dmxPresets = nullptr);
    ~WebServer();

    esp_err_t init();
    esp_err_t start();
    esp_err_t stop();
    void setDmxPresets(DmxPresets *dmxPresets) { dmxPresets_ = dmxPresets; }

    // Post an event to the web server task
    void postEvent(const WebServerEvent &event);

private:
    httpd_handle_t server_;
    DmxPresets *dmxPresets_;
    bool initialized_;

    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;

    void init_spiffs();
    static void taskEntry(void *param);
    void taskLoop();

    static esp_err_t root_handler(httpd_req_t *req);
    static esp_err_t api_presets_handler(httpd_req_t *req);
    static esp_err_t api_config_handler(httpd_req_t *req);
    static esp_err_t static_file_handler(httpd_req_t *req);

    esp_err_t send_json_response(httpd_req_t *req, const char *json);
    esp_err_t send_error_response(httpd_req_t *req, int status, const char *message);
    std::string presets_to_json();
    esp_err_t json_to_presets(const char *json);
    std::string config_to_json();
    esp_err_t json_to_config(const char *json, FootSwitch *footSwitch);

    static WebServer *instance_;
};
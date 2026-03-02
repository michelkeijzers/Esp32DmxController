#pragma once
#include "Data/dmx_presets.hpp"
#include "foot_switch.hpp"
#include <esp_err.h>
#include <esp_http_server.h>
#include <string>

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
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

    WebServer();
    virtual ~WebServer();

    virtual esp_err_t init();

  public:
    std::string presets_to_json();

  public:
    DmxPresets *dmxPresets_ = nullptr;

  private:
    const char *logTag_;
    int taskPriority_;
    int queueCapacity_;
    esp_err_t start();
    esp_err_t stop();

    // Post an event to the web server task
    void postEvent(const WebServerEvent &event);

  private:
    httpd_handle_t server_;
    bool initialized_;

    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;

    void init_spiffs();
    static void taskEntry(void *param);
    void taskLoop();

    static esp_err_t root_handler(httpd_req_t *req);
    static esp_err_t api_presets_handler(httpd_req_t *req);
    static esp_err_t api_config_handler(httpd_req_t *req);
    static esp_err_t api_all_data_handler(httpd_req_t *req);
    static esp_err_t static_file_handler(httpd_req_t *req);

    esp_err_t send_json_response(httpd_req_t *req, const char *json);
    esp_err_t send_error_response(httpd_req_t *req, int status, const char *message);
    // removed duplicate declaration
    esp_err_t json_to_presets(const char *json);
    std::string config_to_json();
    esp_err_t json_to_config(const char *json, FootSwitch *footSwitch);

    static WebServer *instance_;

  public:
    void test_cjson_logic();
    void test_presets_to_json();
    void test_cjson_add_item();
};
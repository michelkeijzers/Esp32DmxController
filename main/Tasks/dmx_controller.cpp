#include "dmx_controller.hpp"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "foot_switch.hpp"
#include "max3485_sender.hpp"
#include "messages.hpp"
#include "nv_storage.hpp"
#include "seven_segment_display.hpp"
#include "web_server.hpp"

DmxController::DmxController(SevenSegmentDisplay *display, FootSwitch *footSwitch, Max3485Sender *max3485Sender,
    WebServer *webServer, NvStorage *nvStorage)
    : RtosTask(), display_(display), footSwitch_(footSwitch), max3485Sender_(max3485Sender), webServer_(webServer),
      nvStorage_(nvStorage)
{
}

DmxController::~DmxController() {}

void DmxController::init()
{
    printf("Initializing DmxController...\n");
    QueueHandle_t queue = getEventQueue(); // Unused
    printf("DmxController init: queue handle = %p\n", (void *)queue);
    setMainEventQueue(queue);
    TaskProperties dmxControllerTaskProperties =
        CreateTaskProperties("DmxControllerTask", "DmxController", 4, 4096, 20, sizeof(Messages::Event));

    RtosTask::init(dmxControllerTaskProperties);

    bootTime = xTaskGetTickCount();
    logFirmwareInfo();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(log_tag_, "Initializing sub-tasks...\n");
    initSubTasks();
    initMessages();
}

void DmxController::initSubTasks()
{
    if (!display_)
    {
        ESP_LOGE(log_tag_, "display_ is nullptr");
        return;
    }
    TaskProperties taskProperties =
        CreateTaskProperties("SevenSegmentDisplayTask", "SevenSegmentDisplay", 5, 4096, 20, sizeof(Messages::Event));
    display_->init(taskProperties, DISPLAY_PINS);

    if (!footSwitch_)
    {
        ESP_LOGE(log_tag_, "footSwitch_ is nullptr");
        return;
    }
    TaskProperties footSwitchTaskProperties =
        CreateTaskProperties("FootSwitchTask", "FootSwitch", 5, 4096, 20, sizeof(Messages::Event));
    footSwitch_->init(footSwitchTaskProperties, FOOT_SWITCH_PIN);

    if (!webServer_)
    {
        ESP_LOGE(log_tag_, "webServer_ is nullptr");
        return;
    }
    TaskProperties webServerTaskProperties =
        CreateTaskProperties("WebServerTask", "WebServer", 5, 4096, 20, sizeof(Messages::Event));
    webServer_->init(webServerTaskProperties);

    if (!max3485Sender_)
    {
        ESP_LOGE(log_tag_, "max3485Sender_ is nullptr");
        return;
    }
    TaskProperties max3485SenderTaskProperties =
        CreateTaskProperties("Max3485SenderTask", "Max3485Sender", 5, 4096, 20, sizeof(Messages::Event));
    max3485Sender_->init(max3485SenderTaskProperties);
}

void DmxController::initMessages()
{
    // Send a message to NvStorage to request config
    Messages::Event event = Messages::Event();
    event.type = Messages::REQUEST_CONFIGURATION;
    if (xQueueSend(nvStorage_->getEventQueue(), &event, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send configuration request to NvStorage");
        return;
    }

    // Receive config response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(log_tag_, "Failed to receive configuration response from NvStorage");
        return;
    }
    if (event.type != Messages::EventType::CONFIGURATION_RESPONSE)
    {
        ESP_LOGE(log_tag_, "Received unexpected configuration event type from NvStorage: %d", event.type);
        return;
    }

    // Send config response to FootSwitch (no response needed)
    Messages::Event footSwitchEvent = Messages::Event();
    footSwitchEvent.type = Messages::SET_CONFIGURATION;
    footSwitchEvent.data.configurationData = event.data.configurationData;
    if (xQueueSend(footSwitch_->getEventQueue(), &footSwitchEvent, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send configuration to FootSwitch");
        return;
    }

    // Send a message to NvStorage to request presets
    event.type = Messages::REQUEST_PRESETS;
    if (xQueueSend(nvStorage_->getEventQueue(), &event, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send presets request to NvStorage");
        return;
    }

    // Receive presets response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(log_tag_, "Failed to receive presets response from NvStorage");
        return;
    }
    if (event.type != Messages::EventType::PRESETS_RESPONSE)
    {
        ESP_LOGE(log_tag_, "Received unexpected event type from NvStorage: %d", event.type);
        return;
    }
}

void DmxController::taskLoop()
{
    while (true)
    {
        // TODO: call performOtaUpdate, etc.

        // Handle next preset event from FootSwitch
        Messages::Event event;
        if (xQueueReceive(getEventQueue(), &event, 0) == pdTRUE)
        {
            switch (event.type)
            {
            case Messages::EventType::USER_NEXT_PRESET:
            {
            }
            break;

            case Messages::EventType::USER_PREVIOUS_PRESET:
            {
            }
            break;

            case Messages::EventType::USE_PRESET_DATA:
            {
                // TODO: Send preset data to DMX output (via Max3485Sender).)
            }
            break;

            case Messages::EventType::SEND_PRESET_DATA_RESPONSE:
            {
                // Forward preset index to SevenSegmentDisplay
                Messages::Event displayEvent = Messages::Event();
                displayEvent.type = Messages::EventType::SHOW_PRESET_INDEX;
                displayEvent.data.presetData.presetNumber = event.data.presetData.presetNumber;
                if (xQueueSend(display_->getEventQueue(), &displayEvent, 0) != pdPASS)
                {
                    ESP_LOGE(log_tag_, "Failed to forward preset index to SevenSegmentDisplay");
                }
            }
            break;

            default:
                // Ignore other events
                break;
            }

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void DmxController::taskEntry(void *param) { static_cast<DmxController *>(param)->taskLoop(); }

void DmxController::logFirmwareInfo()
{
    const esp_app_desc_t *app_desc = esp_app_get_description();
    ESP_LOGW(log_tag_, "Current firmware version: %s", app_desc->version);
    ESP_LOGW(log_tag_, "Project name: %s", app_desc->project_name);
    ESP_LOGW(log_tag_, "Compile time: %s %s", app_desc->date, app_desc->time);
}

esp_err_t DmxController::performOtaUpdate(const char *url)
{
    printf("Starting OTA update from: %s\n", url);
    esp_https_ota_config_t ota_config = {};
    // ota_config.http_config = NULL;
    // ota_config.http_client_init_cb = NULL;
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        ESP_LOGW(log_tag_, "OTA update successful, restarting...");
        esp_restart();
    }
    else
    {
        ESP_LOGE(log_tag_, "OTA update failed: %s\n", esp_err_to_name(ret));
    }
    return ret;
}

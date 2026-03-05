#include "dmx_controller.hpp"
#include "../Base/assert.hpp"
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

DmxController::DmxController(IAssert *assert, Configuration &configuration, DmxPresets &dmxPresets,
    SevenSegmentDisplay *display, FootSwitch *footSwitch, Max3485Sender *max3485Sender, WebServer *webServer,
    NvStorage *nvStorage)
    : RtosTask(), display_(display), footSwitch_(footSwitch), max3485Sender_(max3485Sender), webServer_(webServer),
      nvStorage_(nvStorage), configuration_(configuration), dmxPresets_(dmxPresets), assert_(assert)
{
}

DmxController::~DmxController() {}

void DmxController::init()
{
    ESP_LOGI(pcTaskGetName(nullptr), "Initializing DmxController...\n");
    QueueHandle_t queue = getEventQueue(); // Unused
    setMainEventQueue(queue);
    TaskProperties dmxControllerTaskProperties =
        CreateTaskProperties("DmxControllerTask", 4, 4096, 20, sizeof(Messages::Event));
    RtosTask::init(dmxControllerTaskProperties);

    logFirmwareInfo();

    assert_->assertNotEspError(esp_netif_init(), "Failed to initialize network interface");
    assert_->assertNotEspError(esp_event_loop_create_default(), "Failed to create default event loop");

    initSubTasks();
    initMessages();
    ESP_LOGI(pcTaskGetName(nullptr), "Initializing DmxController completed.\n");
}

void DmxController::initSubTasks()
{
    ESP_LOGI(pcTaskGetName(nullptr), "Initializing DmxController subtasks...\n");

    assert_->assertNotNull(display_, "SevenSegmentDisplay");
    TaskProperties taskProperties =
        CreateTaskProperties("SevenSegmentDisplayTask", 5, 4096, 20, sizeof(Messages::Event));
    display_->init(taskProperties, DISPLAY_PINS);

    assert_->assertNotNull(footSwitch_, "FootSwitch");
    TaskProperties footSwitchTaskProperties =
        CreateTaskProperties("FootSwitchTask", 5, 4096, 20, sizeof(Messages::Event));
    footSwitch_->init(footSwitchTaskProperties, FOOT_SWITCH_PIN);

    assert_->assertNotNull(webServer_, "WebServer");
    TaskProperties webServerTaskProperties =
        CreateTaskProperties("WebServerTask", 5, 4096, 20, sizeof(Messages::Event));
    webServer_->init(webServerTaskProperties);

    assert_->assertNotNull(max3485Sender_, "Max3485Sender");
    TaskProperties max3485SenderTaskProperties =
        CreateTaskProperties("Max3485SenderTask", 5, 4096, 20, sizeof(Messages::Event));
    max3485Sender_->init(max3485SenderTaskProperties);

    ESP_LOGI(pcTaskGetName(nullptr), "Initializing DmxController subtasks completed.\n");
}

void DmxController::initMessages()
{
    ESP_LOGI(pcTaskGetName(nullptr), "Initializing DmxController messages...\n");

    Messages::Event event = Messages::Event();
    event.type = Messages::LOAD_CONFIGURATION;

    assert_->assertPdPass(
        xQueueSend(nvStorage_->getEventQueue(), &event, 0), "Failed to send LOAD_CONFIGURATION message to NvStorage");

    event.type = Messages::LOAD_DMX_PRESETS;

    assert_->assertPdPass(
        xQueueSend(nvStorage_->getEventQueue(), &event, 0), "Failed to send LOAD_DMX_PRESETS message to NvStorage");

    ESP_LOGI(pcTaskGetName(nullptr), "Initializing DmxController messages completed.\n");
}

void DmxController::taskLoop()
{
    while (true)
    {
        Messages::Event event;
        if (xQueueReceive(getEventQueue(), &event, 0) == pdTRUE)
        {
            switch (event.type)
            {
            case Messages::EventType::CONFIGURATION_LOADED:
            {
                Messages::Event updateConfigurationEvent = Messages::Event();
                updateConfigurationEvent.type = Messages::EventType::UPDATE_CONFIGURATION;

                assert_->assertPdPass(xQueueSend(footSwitch_->getEventQueue(), &updateConfigurationEvent, 0),
                    "Failed to forward configuration loaded to FootSwitch");

                Messages::Event updateConfigurationWebEvent = Messages::Event();
                updateConfigurationWebEvent.type = Messages::EventType::UPDATE_CONFIGURATION;

                assert_->assertPdPass(xQueueSend(webServer_->getEventQueue(), &updateConfigurationWebEvent, 0),
                    "Failed to forward configuration loaded to WebServer");
            }
            break;

            // TODO: ERROR (from NVStorage)
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
    ESP_LOGW(pcTaskGetName(nullptr), "Current firmware version: %s", app_desc->version);
    ESP_LOGW(pcTaskGetName(nullptr), "Project name: %s", app_desc->project_name);
    ESP_LOGW(pcTaskGetName(nullptr), "Compile time: %s %s", app_desc->date, app_desc->time);
}

esp_err_t DmxController::performOtaUpdate(const char *url)
{
    ESP_LOGI(pcTaskGetName(nullptr), "Starting OTA update from: %s", url);
    esp_https_ota_config_t ota_config = {};
    // ota_config.http_config = NULL;
    // ota_config.http_client_init_cb = NULL;
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        ESP_LOGW(pcTaskGetName(nullptr), "OTA update successful, restarting...");
        esp_restart();
    }
    else
    {
        ESP_LOGE(pcTaskGetName(nullptr), "OTA update failed: %s\n", esp_err_to_name(ret));
    }
    return ret;
}

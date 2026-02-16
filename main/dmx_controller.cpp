#include "dmx_controller.hpp"
#include "artnet_sender.hpp"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "messages.hpp"

static const char *LOG_TAG = "DmxController";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

DmxController::DmxController(DmxPresetChanger *presetChanger, OSCSender *oscSender, SevenSegmentDisplay *display,
    FootSwitch *footSwitch, ArtNetSender *artnetSender, WebServer *webServer, NvsStorage *nvsStorage)
    : RtosTask(), presetChanger_(presetChanger), oscSender_(oscSender), display_(display), footSwitch_(footSwitch),
      artnetSender_(artnetSender), webServer_(webServer), nvsStorage_(nvsStorage)
{
}

DmxController::~DmxController()
{
    delete presetChanger_;
    delete oscSender_;
    delete display_;
    delete footSwitch_;
    delete artnetSender_;
    delete webServer_;
}

void DmxController::printFirmwareInfo()
{
    const esp_app_desc_t *app_desc = esp_app_get_description();
    ESP_LOGW(LOG_TAG, "Current firmware version: %s\n", app_desc->version);
    ESP_LOGW(LOG_TAG, "Project name: %s\n", app_desc->project_name);
    ESP_LOGW(LOG_TAG, "Compile time: %s %s\n", app_desc->date, app_desc->time);
}

esp_err_t DmxController::performOtaUpdate(const char *url)
{
    printf("Starting OTA update from: %s\n", url);
    // Classic C++ initialization for unit test build
    esp_http_client_config_t config = {};
    // If needed, set fields manually here for test build
    // config.url = url; // Not available in stub, so skip or add to stub if needed
    // config.cert_pem = OTA_CERT_PEM;
    // config.timeout_ms = 30000;
    // config.skip_cert_common_name_check = false;

    esp_https_ota_config_t ota_config = {};
    // ota_config.http_config = &config;
    // ota_config.http_client_init_cb = NULL;
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        ESP_LOGW(LOG_TAG, "OTA update successful, restarting...\n");
        esp_restart();
    }
    else
    {
        ESP_LOGE(LOG_TAG, "OTA update failed: %s\n", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t DmxController::init()
{
    printf("Initializing DmxController...\n");
    QueueHandle_t queue = getEventQueue(); // Unused
    printf("DmxController init: queue handle = %p\n", (void *)queue);
    if (RtosTask::init("DmxControllerTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Messages::Event), queue) !=
        ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxControllerTask");
        return ESP_FAIL;
    }

    printf("DmxController initialized successfully, bootTime=%lu\n", bootTime);

    bootTime = xTaskGetTickCount();
    printFirmwareInfo();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    printf("Initializing sub-tasks...\n");
    if (init_sub_tasks() != ESP_OK)
    {
        printf("Failed to initialize sub-tasks\n");
        ESP_LOGE(LOG_TAG, "Failed to initialize sub-tasks");
        return ESP_FAIL;
    }

    if (init_messages() != ESP_OK)
    {
        printf("Failed to initialize message handling\n");
        ESP_LOGE(LOG_TAG, "Failed to initialize message handling");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DmxController::init_sub_tasks()
{
    if (!presetChanger_)
        presetChanger_ = new DmxPresetChanger();
    if (presetChanger_->init(getEventQueue()) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxPresetChanger");
        return ESP_FAIL;
    }

    if (!oscSender_)
        oscSender_ = new OSCSender();
    if (oscSender_->init(OSC_DEST_IP, OSC_DEST_PORT) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize OSCSender");
        return ESP_FAIL;
    }

    if (!display_)
        display_ = new SevenSegmentDisplay();
    if (display_->init(getEventQueue(), DISPLAY_PINS) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize SevenSegmentDisplay");
        return ESP_FAIL;
    }

    if (!footSwitch_)
        footSwitch_ = new FootSwitch();
    if (footSwitch_->init(getEventQueue(), FOOT_SWITCH_PIN) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize FootSwitch");
        return ESP_FAIL;
    }

    if (!artnetSender_)
        artnetSender_ = new ArtNetSender();
    if (artnetSender_->init(getEventQueue(), ARTNET_DEST_IP, 6454) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize ArtNetSender");
        return ESP_FAIL;
    }

    if (!webServer_)
        webServer_ = new WebServer();
    if (webServer_->init() != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize WebServer");
        return ESP_FAIL;
    }

    if (!nvsStorage_)
        nvsStorage_ = new NvsStorage();
    if (nvsStorage_->init(getEventQueue()) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize NvsStorage");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DmxController::init_messages()
{
    // Send a message to NvsStorage to request config
    Messages::Event event = Messages::Event();
    event.type = Messages::REQUEST_CONFIGURATION;
    if (xQueueSend(nvsStorage_->getEventQueue(), &event, 0) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Failed to send configuration request to NvsStorage");
        return ESP_FAIL;
    }

    // Receive config response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(LOG_TAG, "Failed to receive configuration response from NvsStorage");
        return ESP_FAIL;
    }
    if (event.type != Messages::EventType::CONFIGURATION_RESPONSE)
    {
        ESP_LOGE(LOG_TAG, "Received unexpected configuration event type from NvsStorage: %d", event.type);
        return ESP_FAIL;
    }

    // Send config response to FootSwitch (no response needed)
    Messages::Event footSwitchEvent = Messages::Event();
    footSwitchEvent.type = Messages::SET_CONFIGURATION;
    footSwitchEvent.data.configurationData = event.data.configurationData;
    if (xQueueSend(footSwitch_->getEventQueue(), &footSwitchEvent, 0) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Failed to send configuration to FootSwitch");
        return ESP_FAIL;
    }

    // Send a message to NvsStorage to request presets
    event.type = Messages::REQUEST_PRESETS;
    if (xQueueSend(nvsStorage_->getEventQueue(), &event, 0) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Failed to send presets request to NvsStorage");
        return ESP_FAIL;
    }

    // Receive presets response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(LOG_TAG, "Failed to receive presets response from NvsStorage");
        return ESP_FAIL;
    }
    if (event.type != Messages::EventType::PRESETS_RESPONSE)
    {
        ESP_LOGE(LOG_TAG, "Received unexpected event type from NvsStorage: %d", event.type);
        return ESP_FAIL;
    }

    // Send presets to DmxPresetChanger (no response needed)
    Messages::Event presetChangerEvent = Messages::Event();
    presetChangerEvent.type = Messages::SET_PRESETS;
    presetChangerEvent.data.presetsData = event.data.presetsData;
    if (xQueueSend(presetChanger_->getEventQueue(), &presetChangerEvent, 0) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Failed to send presets to DmxPresetChanger");
        return ESP_FAIL;
    }

    return ESP_OK;
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
                // Forward to DmxPresetChanger
                Messages::Event presetChangerEvent = Messages::Event();
                presetChangerEvent.type = Messages::EventType::SELECT_NEXT_PRESET;
                if (xQueueSend(presetChanger_->getEventQueue(), &presetChangerEvent, 0) != pdPASS)
                {
                    ESP_LOGE(LOG_TAG, "Failed to forward next preset change event to DmxPresetChanger");
                }
            }
            break;

            case Messages::EventType::USER_PREVIOUS_PRESET:
            {
                // Forward to DmxPresetChanger
                Messages::Event presetChangerEvent = Messages::Event();
                presetChangerEvent.type = Messages::EventType::SELECT_PREVIOUS_PRESET;
                if (xQueueSend(presetChanger_->getEventQueue(), &presetChangerEvent, 0) != pdPASS)
                {
                    ESP_LOGE(LOG_TAG, "Failed to forward previous preset change event to DmxPresetChanger");
                }
            }
            break;

            case Messages::EventType::USE_PRESET_DATA:
            {
                // Forward to ArtNetSender
                Messages::Event artNetEvent = Messages::Event();
                artNetEvent.type = Messages::EventType::SEND_PRESET_DATA;
                artNetEvent.data.presetData = event.data.presetData;
                if (xQueueSend(artnetSender_->getEventQueue(), &artNetEvent, 0) != pdPASS)
                {
                    ESP_LOGE(LOG_TAG, "Failed to forward preset data to ArtNetSender");
                }
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
                    ESP_LOGE(LOG_TAG, "Failed to forward preset index to SevenSegmentDisplay");
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

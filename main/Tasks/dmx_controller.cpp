#include "dmx_controller.hpp"
#include "dmx_preset_changer.hpp"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "foot_switch.hpp"
#include "max3485_sender.hpp"
#include "messages.hpp"
#include "nv_storage.hpp"
#include "osc_sender.hpp"
#include "seven_segment_display.hpp"
#include "web_server.hpp"

DmxController::DmxController(DmxPresetChanger *presetChanger, OSCSender *oscSender, SevenSegmentDisplay *display,
    FootSwitch *footSwitch, Max3485Sender *max3485Sender, WebServer *webServer, NvStorage *nvStorage)
    : RtosTask(), presetChanger_(presetChanger), oscSender_(oscSender), display_(display), footSwitch_(footSwitch),
      max3485Sender_(max3485Sender), webServer_(webServer), nvStorage_(nvStorage)
{
}

DmxController::~DmxController() {}

void DmxController::printFirmwareInfo()
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

esp_err_t DmxController::init()
{
    printf("Initializing DmxController...\n");
    QueueHandle_t queue = getEventQueue(); // Unused
    printf("DmxController init: queue handle = %p\n", (void *)queue);
    TaskProperties dmxControllerTaskProperties = {.taskName_ = "DmxControllerTask",
        .logTag = "DmxController",
        .taskPriority = 4,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = queue};

    if (RtosTask::init(dmxControllerTaskProperties) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize DmxControllerTask");
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
        ESP_LOGE(log_tag_, "Failed to initialize sub-tasks");
        return ESP_FAIL;
    }

    if (init_messages() != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize message handling");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DmxController::init_sub_tasks()
{
    if (!presetChanger_)
    {
        ESP_LOGE(log_tag_, "presetChanger_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    TaskProperties presetChangerTaskProperties = {.taskName_ = "DmxPresetChangerTask",
        .logTag = "DmxPresetChanger",
        .taskPriority = 5,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = getEventQueue()};
    if (presetChanger_->init(presetChangerTaskProperties) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize DmxPresetChanger");
        return ESP_FAIL;
    }

    if (!oscSender_)
    {
        ESP_LOGE(log_tag_, "oscSender_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    TaskProperties oscSenderTaskProperties = {.taskName_ = "OSCSenderTask",
        .logTag = "OSCSender",
        .taskPriority = 5,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = getEventQueue()};
    if (oscSender_->init(oscSenderTaskProperties, OSC_DEST_IP, OSC_DEST_PORT) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize OSCSender");
        return ESP_FAIL;
    }

    if (!display_)
    {
        ESP_LOGE(log_tag_, "display_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    TaskProperties taskProperties = {.taskName_ = "SevenSegmentDisplayTask",
        .logTag = "SevenSegmentDisplay",
        .taskPriority = 5,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = getEventQueue()};
    if (display_->init(taskProperties, DISPLAY_PINS) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize SevenSegmentDisplay");
        return ESP_FAIL;
    }

    if (!footSwitch_)
    {
        ESP_LOGE(log_tag_, "footSwitch_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    TaskProperties footSwitchTaskProperties = {.taskName_ = "FootSwitchTask",
        .logTag = "FootSwitch",
        .taskPriority = 5,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = getEventQueue()};
    if (footSwitch_->init(footSwitchTaskProperties, FOOT_SWITCH_PIN) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize FootSwitch");
        return ESP_FAIL;
    }

    if (!max3485Sender_)
    {
        ESP_LOGE(log_tag_, "max3485Sender_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    TaskProperties max3485SenderTaskProperties = {.taskName_ = "Max3485SenderTask",
        .logTag = "Max3485Sender",
        .taskPriority = 5,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = getEventQueue()};
    if (max3485Sender_->init(max3485SenderTaskProperties) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize Max3485Sender");
        return ESP_FAIL;
    }

    if (!webServer_)
    {
        ESP_LOGE(log_tag_, "webServer_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    if (webServer_->init() != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize WebServer");
        return ESP_FAIL;
    }

    TaskProperties nvStorageTaskProperties = {.taskName_ = "NvStorageTask",
        .logTag = "NvStorage",
        .taskPriority = 5,
        .stackSize = 4096,
        .queueCapacity = 20,
        .queueItemSize = sizeof(Messages::Event),
        .mainEventQueue = getEventQueue()};
    if (!nvStorage_)
    {
        ESP_LOGE(log_tag_, "nvStorage_ is nullptr");
        return ESP_ERR_INVALID_ARG;
    }
    if (nvStorage_->init(nvStorageTaskProperties) != ESP_OK)
    {
        ESP_LOGE(log_tag_, "Failed to initialize NvStorage");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DmxController::init_messages()
{
    // Send a message to NvStorage to request config
    Messages::Event event = Messages::Event();
    event.type = Messages::REQUEST_CONFIGURATION;
    if (xQueueSend(nvStorage_->getEventQueue(), &event, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send configuration request to NvStorage");
        return ESP_FAIL;
    }

    // Receive config response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(log_tag_, "Failed to receive configuration response from NvStorage");
        return ESP_FAIL;
    }
    if (event.type != Messages::EventType::CONFIGURATION_RESPONSE)
    {
        ESP_LOGE(log_tag_, "Received unexpected configuration event type from NvStorage: %d", event.type);
        return ESP_FAIL;
    }

    // Send config response to FootSwitch (no response needed)
    Messages::Event footSwitchEvent = Messages::Event();
    footSwitchEvent.type = Messages::SET_CONFIGURATION;
    footSwitchEvent.data.configurationData = event.data.configurationData;
    if (xQueueSend(footSwitch_->getEventQueue(), &footSwitchEvent, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send configuration to FootSwitch");
        return ESP_FAIL;
    }

    // Send a message to NvStorage to request presets
    event.type = Messages::REQUEST_PRESETS;
    if (xQueueSend(nvStorage_->getEventQueue(), &event, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send presets request to NvStorage");
        return ESP_FAIL;
    }

    // Receive presets response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(log_tag_, "Failed to receive presets response from NvStorage");
        return ESP_FAIL;
    }
    if (event.type != Messages::EventType::PRESETS_RESPONSE)
    {
        ESP_LOGE(log_tag_, "Received unexpected event type from NvStorage: %d", event.type);
        return ESP_FAIL;
    }

    // Send presets to DmxPresetChanger (no response needed)
    Messages::Event presetChangerEvent = Messages::Event();
    presetChangerEvent.type = Messages::SET_PRESETS;
    presetChangerEvent.data.presetsData = event.data.presetsData;
    if (xQueueSend(presetChanger_->getEventQueue(), &presetChangerEvent, 0) != pdPASS)
    {
        ESP_LOGE(log_tag_, "Failed to send presets to DmxPresetChanger");
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
                    ESP_LOGE(log_tag_, "Failed to forward next preset change event to DmxPresetChanger");
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
                    ESP_LOGE(log_tag_, "Failed to forward previous preset change event to DmxPresetChanger");
                }
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

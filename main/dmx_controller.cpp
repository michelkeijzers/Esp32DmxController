#include "dmx_controller.hpp"
#include "messages.hpp"

static const char *LOG_TAG = "DmxController";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

DmxController::DmxController() : RtosTask() {}

DmxController::~DmxController() {
    delete presetChanger;
    delete oscSender;
    delete display;
    delete footSwitch;
    delete artnetSender;
    delete webServer;
}

void DmxController::printFirmwareInfo() {
    const esp_app_desc_t *app_desc = esp_app_get_description();
    ESP_LOGW(LOG_TAG, "Current firmware version: %s\n", app_desc->version);
    ESP_LOGW(LOG_TAG, "Project name: %s\n", app_desc->project_name);
    ESP_LOGW(LOG_TAG, "Compile time: %s %s\n", app_desc->date, app_desc->time);
}

esp_err_t DmxController::performOtaUpdate(const char *url) {
    printf("Starting OTA update from: %s\n", url);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = OTA_CERT_PEM,
        .timeout_ms = 30000,
        .skip_cert_common_name_check = false,
    };
#pragma GCC diagnostic pop
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    esp_https_ota_config_t ota_config = {.http_config = &config, .http_client_init_cb = NULL};
#pragma GCC diagnostic pop
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGW(LOG_TAG, "OTA update successful, restarting...\n");
        esp_restart();
    } else {
        ESP_LOGE(LOG_TAG, "OTA update failed: %s\n", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t DmxController::init() {
    QueueHandle_t queue = getEventQueue(); // Unused
    if (RtosTask::init("DmxControllerTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Messages::Event), queue) !=
        ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxControllerTask");
        return ESP_FAIL;
    }

    bootTime = xTaskGetTickCount();
    printFirmwareInfo();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    if (init_sub_tasks() != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize sub-tasks");
        return ESP_FAIL;
    }

    if (init_messages() != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize message handling");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DmxController::init_sub_tasks() {
    presetChanger = new DmxPresetChanger();
    if (presetChanger->init(getEventQueue()) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxPresetChanger");
        return ESP_FAIL;
    }

    oscSender = new OSCSender();
    if (oscSender->init(OSC_DEST_IP, OSC_DEST_PORT) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize OSCSender");
        return ESP_FAIL;
    }

    display = new SevenSegmentDisplay();
    if (display->init(getEventQueue(), DISPLAY_PINS) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize SevenSegmentDisplay");
        return ESP_FAIL;
    }

    footSwitch = new FootSwitch();
    if (footSwitch->init(getEventQueue(), FOOT_SWITCH_PIN) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize FootSwitch");
        return ESP_FAIL;
    }

    artnetSender = new ArtNetSender();
    if (artnetSender->init(getEventQueue(), ARTNET_DEST_IP, 6454) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize ArtNetSender");
        return ESP_FAIL;
    }

    webServer = new WebServer();
    if (webServer->init() != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize WebServer");
        return ESP_FAIL;
    }

    nvsStorage = new NvsStorage();
    if (nvsStorage->init(getEventQueue()) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize NvsStorage");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t DmxController::init_messages() {
    // Send a message to NvsStorage to request config
    Messages::Event event = Messages::Event();
    event.type = Messages::REQUEST_CONFIGURATION;
    if (xQueueSend(nvsStorage->getEventQueue(), &event, 0) != pdPASS) {
        ESP_LOGE(LOG_TAG, "Failed to send configuration request to NvsStorage");
        return ESP_FAIL;
    }

    // Receive config response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(LOG_TAG, "Failed to receive configuration response from NvsStorage");
        return ESP_FAIL;
    }
    if (event.type != Messages::EventType::CONFIGURATION_RESPONSE) {
        ESP_LOGE(LOG_TAG, "Received unexpected configuration event type from NvsStorage: %d", event.type);
        return ESP_FAIL;
    }

    // Send config response to FootSwitch (no response needed)
    Messages::Event footSwitchEvent = Messages::Event();
    footSwitchEvent.type = Messages::SET_CONFIGURATION;
    footSwitchEvent.data.configurationData = event.data.configurationData;
    if (xQueueSend(footSwitch->getEventQueue(), &footSwitchEvent, 0) != pdPASS) {
        ESP_LOGE(LOG_TAG, "Failed to send configuration to FootSwitch");
        return ESP_FAIL;
    }

    // Send a message to NvsStorage to request presets
    event.type = Messages::REQUEST_PRESETS;
    if (xQueueSend(nvsStorage->getEventQueue(), &event, 0) != pdPASS) {
        ESP_LOGE(LOG_TAG, "Failed to send presets request to NvsStorage");
        return ESP_FAIL;
    }

    // Receive presets response (blocking)
    if (xQueueReceive(getEventQueue(), &event, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(LOG_TAG, "Failed to receive presets response from NvsStorage");
        return ESP_FAIL;
    }
    if (event.type != Messages::EventType::PRESETS_RESPONSE) {
        ESP_LOGE(LOG_TAG, "Received unexpected event type from NvsStorage: %d", event.type);
        return ESP_FAIL;
    }

    // Send presets to DmxPresetChanger (no response needed)
    Messages::Event presetChangerEvent = Messages::Event();
    presetChangerEvent.type = Messages::SET_PRESETS;
    presetChangerEvent.data.presetsData = event.data.presetsData;
    if (xQueueSend(presetChanger->getEventQueue(), &presetChangerEvent, 0) != pdPASS) {
        ESP_LOGE(LOG_TAG, "Failed to send presets to DmxPresetChanger");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void DmxController::taskLoop() {
    while (true) {
        // TODO: Handle FootSwitchEventType events, call performOtaUpdate, etc.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void DmxController::taskEntry(void *param) { static_cast<DmxController *>(param)->taskLoop(); }

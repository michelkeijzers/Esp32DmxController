#include "dmx_controller.hpp"

static const char *LOG_TAG = "DmxController";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

DmxController::DmxController()
    : RtosTask()
{
}

DmxController::~DmxController()
{
    delete presetChanger;
    delete oscSender;
    delete dmxPresets;
    delete display;
    delete footSwitch;
    delete artnetSender;
    delete webServer;
}

void DmxController::printFirmwareInfo()
{
    const esp_app_desc_t *app_desc = esp_app_get_description();
    printf("Current firmware version: %s\n", app_desc->version);
    printf("Project name: %s\n", app_desc->project_name);
    printf("Compile time: %s %s\n", app_desc->date, app_desc->time);
}

esp_err_t DmxController::performOtaUpdate(const char *url)
{
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
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
        .http_client_init_cb = NULL};
#pragma GCC diagnostic pop
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        printf("OTA update successful, restarting...\n");
        esp_restart();
    }
    else
    {
        printf("OTA update failed: %s\n", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t DmxController::init()
{
    if (RtosTask::init("DmxControllerTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(DmxControllerEvent)) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to initialize DmxControllerTask");
        return ESP_FAIL;
    }

    bootTime = xTaskGetTickCount();
    printFirmwareInfo();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    dmxPresets = new DmxPresets();
    if (dmxPresets->init() != ESP_OK)
    {
        printf("Failed to initialize DMX presets\n");
        return ESP_FAIL;
    }
    webServer = new WebServer(dmxPresets);
    if (webServer->init() == ESP_OK)
    {
        printf("Web server initialized\n");
    }
    else
    {
        printf("Failed to initialize web server\n");
    }
    display = new SevenSegmentDisplay();
    if (display->init(DISPLAY_PINS) != ESP_OK)
    {
        printf("Failed to initialize 7-segment display\n");
        return ESP_FAIL;
    }
    oscSender = new OSCSender();
    if (oscSender->init(OSC_DEST_IP, OSC_DEST_PORT) == ESP_OK)
    {
        printf("OSC sender initialized to %s:%d\n", OSC_DEST_IP, OSC_DEST_PORT);
    }
    else
    {
        printf("Failed to initialize OSC sender\n");
    }
    artnetSender = new ArtNetSender();
    if (artnetSender->init(ARTNET_DEST_IP) != ESP_OK)
    {
        printf("Failed to initialize Art-Net sender\n");
        return ESP_FAIL;
    }
    presetChanger = new DmxPresetChanger();
    if (presetChanger->init() != ESP_OK)
    {
        printf("Failed to initialize DMX preset changer\n");
        return ESP_FAIL;
    }

    footSwitch = new FootSwitch();
    if (footSwitch->init(FOOT_SWITCH_PIN) != ESP_OK)
    {
        printf("Failed to initialize foot switch\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void DmxController::taskLoop()
{
    while (true)
    {
        // TODO: Handle FootSwitchEventType events, call performOtaUpdate, etc.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void DmxController::taskEntry(void *param)
{
    static_cast<DmxController *>(param)->taskLoop();
}

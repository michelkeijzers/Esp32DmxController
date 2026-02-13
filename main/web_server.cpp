#include "web_server.hpp"
#include <esp_log.h>

#include "foot_switch.hpp"
#include <cJSON.h>
#include <cstring>
#include <esp_spiffs.h>
#include <vector>

static const char *TAG = "WebServer";

// Static instance pointer for handlers
WebServer *WebServer::instance_ = nullptr;

// HTML content for the React app (this would normally be generated files)
__attribute__((unused)) static const char *INDEX_HTML = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 DMX Controller</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 1200px; margin: 0 auto; }
        .preset { border: 1px solid #ccc; padding: 10px; margin: 10px 0; }
        .universe { display: flex; flex-wrap: wrap; gap: 2px; margin: 10px 0; }
        .channel { width: 20px; height: 20px; border: 1px solid #ddd; display: inline-block; }
        .channel.on { background-color: #ffff00; }
        input, button { margin: 5px; padding: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 DMX Controller</h1>
        <div id="app">
            <p>Loading...</p>
        </div>
    </div>
    <script src="/static/app.js"></script>
</body>
</html>
)html";

// JavaScript content (simplified React-like implementation)
__attribute__((unused)) static const char *APP_JS = R"js(
class DMXController {
    constructor() {
        this.presets = [];
        this.config = { footSwitchPolarity: 'NORMAL' };
        this.currentPreset = 0;
        this.init();
    }

    async init() {
        await this.loadPresets();
        await this.loadConfig();
        this.render();
        this.bindEvents();
    }

    async loadPresets() {
        try {
            const response = await fetch('/api/presets');
            this.presets = await response.json();
        } catch (error) {
            console.error('Failed to load presets:', error);
            this.presets = [];
        }
    }

    async loadConfig() {
        try {
            const response = await fetch('/api/config');
            this.config = await response.json();
        } catch (error) {
            console.error('Failed to load config:', error);
        }
    }

    async savePresets() {
        try {
            await fetch('/api/presets', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(this.presets)
            });
            alert('Presets saved successfully!');
        } catch (error) {
            console.error('Failed to save presets:', error);
            alert('Failed to save presets');
        }
    }

    async saveConfig() {
        try {
            await fetch('/api/config', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(this.config)
            });
            alert('Configuration saved successfully!');
        } catch (error) {
            console.error('Failed to save config:', error);
            alert('Failed to save configuration');
        }
    }

    render() {
        const app = document.getElementById('app');
        app.innerHTML = `
            <div>
                <h2>Configuration</h2>
                <label>
                    Foot Switch Polarity:
                    <select id="footSwitchPolarity">
                        <option value="NORMAL" ${this.config.footSwitchPolarity === 'NORMAL' ? 'selected' : ''}>Normal</option>
                        <option value="INVERSE" ${this.config.footSwitchPolarity === 'INVERSE' ? 'selected' : ''}>Inverse</option>
                    </select>
                </label>
                <button id="saveConfig">Save Configuration</button>
            </div>

            <div>
                <h2>Presets (${this.presets.length})</h2>
                <button id="addPreset">Add Preset</button>
                <button id="savePresets">Save All Presets</button>
                <div id="presetsContainer"></div>
            </div>
        `;

        this.renderPresets();
    }

    renderPresets() {
        const container = document.getElementById('presetsContainer');
        container.innerHTML = this.presets.map((preset, index) => `
            <div class="preset">
                <h3>Preset ${index}: <input type="text" value="${preset.name || ''}" onchange="app.updatePresetName(${index}, this.value)"></h3>
                <button onclick="app.deletePreset(${index})">Delete</button>
                <div>
                    <h4>Universe 1</h4>
                    <div class="universe">
                        ${Array.from({length: 512}, (_, i) => `
                            <div class="channel ${preset.universe1[i] > 0 ? 'on' : ''}"
                                 style="background-color: rgb(${preset.universe1[i]}, ${preset.universe1[i]}, ${preset.universe1[i]})"
                                 onclick="app.toggleChannel(${index}, 0, ${i})">
                            </div>
                        `).join('')}
                    </div>
                </div>
                <div>
                    <h4>Universe 2</h4>
                    <div class="universe">
                        ${Array.from({length: 512}, (_, i) => `
                            <div class="channel ${preset.universe2[i] > 0 ? 'on' : ''}"
                                 style="background-color: rgb(${preset.universe2[i]}, ${preset.universe2[i]}, ${preset.universe2[i]})"
                                 onclick="app.toggleChannel(${index}, 1, ${i})">
                            </div>
                        `).join('')}
                    </div>
                </div>
            </div>
        `).join('');
    }

    bindEvents() {
        document.getElementById('addPreset').onclick = () => this.addPreset();
        document.getElementById('savePresets').onclick = () => this.savePresets();
        document.getElementById('saveConfig').onclick = () => this.saveConfig();
        document.getElementById('footSwitchPolarity').onchange = (e) => {
            this.config.footSwitchPolarity = e.target.value;
        };
    }

    addPreset() {
        if (this.presets.length >= 20) {
            alert('Maximum 20 presets allowed');
            return;
        }
        this.presets.push({
            name: `Preset ${this.presets.length}`,
            universe1: new Array(512).fill(0),
            universe2: new Array(512).fill(0)
        });
        this.render();
    }

    deletePreset(index) {
        if (this.presets.length <= 2) {
            alert('Minimum 2 presets required');
            return;
        }
        this.presets.splice(index, 1);
        this.render();
    }

    updatePresetName(index, name) {
        this.presets[index].name = name;
    }

    toggleChannel(presetIndex, universe, channel) {
        const preset = this.presets[presetIndex];
        const universeData = universe === 0 ? preset.universe1 : preset.universe2;
        universeData[channel] = universeData[channel] > 0 ? 0 : 255;
        this.render();
    }
}

// Initialize the app
const app = new DMXController();
)js";

WebServer::WebServer() : server_(nullptr)
{
    instance_ = this;
    eventQueue_ = xQueueCreate(4, sizeof(WebServerEvent));
    if (eventQueue_)
    {
        xTaskCreate(taskEntry, "WebServerTask", 4096, this, 5, &taskHandle_);
        ESP_LOGI(TAG, "WebServer task started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create WebServer event queue");
    }
}

WebServer::~WebServer()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    if (eventQueue_)
    {
        vQueueDelete(eventQueue_);
    }
}

void WebServer::postEvent(const WebServerEvent &event)
{
    if (eventQueue_)
    {
        xQueueSend(eventQueue_, &event, 0);
    }
}

void WebServer::taskEntry(void *param) { static_cast<WebServer *>(param)->taskLoop(); }

void WebServer::taskLoop()
{
    WebServerEvent event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            switch (event.type)
            {
            case START_SERVER:
                start();
                break;
            case STOP_SERVER:
                stop();
                break;
            case RESTART_SERVER:
                stop();
                start();
                break;
            }
        }
    }
}

void WebServer::init_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs", .partition_label = NULL, .max_files = 8, .format_if_mount_failed = true};
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "SPIFFS mounted successfully");
    }
}

esp_err_t WebServer::init()
{
    if (initialized_)
    {
        return ESP_OK;
    }

    // Mount SPIFFS
    init_spiffs();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 8;

    esp_err_t ret = httpd_start(&server_, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return ret;
    }

    // Register URI handlers
    httpd_uri_t root_uri = {.uri = "/", .method = HTTP_GET, .handler = root_handler, .user_ctx = nullptr};
    httpd_register_uri_handler(server_, &root_uri);

    httpd_uri_t api_presets_uri = {
        .uri = "/api/presets", .method = HTTP_GET, .handler = api_presets_handler, .user_ctx = nullptr};
    httpd_register_uri_handler(server_, &api_presets_uri);

    httpd_uri_t api_presets_post_uri = {
        .uri = "/api/presets", .method = HTTP_POST, .handler = api_presets_handler, .user_ctx = nullptr};
    httpd_register_uri_handler(server_, &api_presets_post_uri);

    httpd_uri_t api_config_uri = {
        .uri = "/api/config", .method = HTTP_GET, .handler = api_config_handler, .user_ctx = nullptr};
    httpd_register_uri_handler(server_, &api_config_uri);

    httpd_uri_t api_config_post_uri = {
        .uri = "/api/config", .method = HTTP_POST, .handler = api_config_handler, .user_ctx = nullptr};
    httpd_register_uri_handler(server_, &api_config_post_uri);

    httpd_uri_t static_file_uri = {
        .uri = "/*", .method = HTTP_GET, .handler = static_file_handler, .user_ctx = nullptr};
    httpd_register_uri_handler(server_, &static_file_uri);

    initialized_ = true;
    ESP_LOGI(TAG, "Web server initialized on port 80");
    return ESP_OK;
}

esp_err_t WebServer::start()
{
    if (!initialized_)
    {
        return init();
    }
    return ESP_OK;
}

esp_err_t WebServer::stop()
{
    if (server_)
    {
        httpd_stop(server_);
        server_ = nullptr;
        initialized_ = false;
    }
    return ESP_OK;
}

esp_err_t WebServer::root_handler(httpd_req_t *req)
{
    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return ESP_FAIL;
    }
    // Serve index.html from SPIFFS
    const char *filepath = "/spiffs/index.html";
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "index.html not found");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "text/html");
    char buffer[1024];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        httpd_resp_send_chunk(req, buffer, read_bytes);
    }
    fclose(file);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t WebServer::api_presets_handler(httpd_req_t *req)
{
    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return ESP_FAIL;
    }

    if (req->method == HTTP_GET)
    {
        // Return presets as JSON
        std::string json = instance_->presets_to_json();
        return instance_->send_json_response(req, json.c_str());
    }
    else if (req->method == HTTP_POST)
    {
        // Parse JSON and update presets
        char content[4096];
        int ret = httpd_req_recv(req, content, sizeof(content));
        if (ret <= 0)
        {
            return instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "No data received");
        }
        content[ret] = '\0';

        esp_err_t err = instance_->json_to_presets(content);
        if (err != ESP_OK)
        {
            return instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        }

        return instance_->send_json_response(req, "{\"status\":\"ok\"}");
    }

    return instance_->send_error_response(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
}

esp_err_t WebServer::api_config_handler(httpd_req_t *req)
{
    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return ESP_FAIL;
    }

    if (req->method == HTTP_GET)
    {
        // Return config as JSON
        std::string json = instance_->config_to_json();
        return instance_->send_json_response(req, json.c_str());
    }
    else if (req->method == HTTP_POST)
    {
        // Parse JSON and update config
        char content[1024];
        int ret = httpd_req_recv(req, content, sizeof(content));
        if (ret <= 0)
        {
            return instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "No data received");
        }
        content[ret] = '\0';

        esp_err_t err = ESP_OK; // TODO esp_err_t err = instance_->json_to_config(content);
        if (err != ESP_OK)
        {
            return instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        }

        return instance_->send_json_response(req, "{\"status\":\"ok\"}");
    }

    return instance_->send_error_response(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
}

esp_err_t WebServer::static_file_handler(httpd_req_t *req)
{
    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return ESP_FAIL;
    }

    // Map URI to SPIFFS file path
    std::string path = "/spiffs";
    if (strcmp(req->uri, "/") == 0)
    {
        path += "/index.html";
    }
    else
    {
        path += req->uri;
    }

    FILE *file = fopen(path.c_str(), "rb");
    if (!file)
    {
        // fallback: serve index.html for unknown routes (SPA)
        file = fopen("/spiffs/index.html", "rb");
        if (!file)
        {
            httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
            return ESP_FAIL;
        }
        httpd_resp_set_type(req, "text/html");
    }
    else
    {
        // Set content type based on file extension
        if (strstr(path.c_str(), ".html"))
            httpd_resp_set_type(req, "text/html");
        else if (strstr(path.c_str(), ".js"))
            httpd_resp_set_type(req, "application/javascript");
        else if (strstr(path.c_str(), ".css"))
            httpd_resp_set_type(req, "text/css");
        else if (strstr(path.c_str(), ".json"))
            httpd_resp_set_type(req, "application/json");
        else if (strstr(path.c_str(), ".png"))
            httpd_resp_set_type(req, "image/png");
        else if (strstr(path.c_str(), ".jpg") || strstr(path.c_str(), ".jpeg"))
            httpd_resp_set_type(req, "image/jpeg");
        else if (strstr(path.c_str(), ".svg"))
            httpd_resp_set_type(req, "image/svg+xml");
        else
            httpd_resp_set_type(req, "application/octet-stream");
    }

    char buffer[1024];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        httpd_resp_send_chunk(req, buffer, read_bytes);
    }
    fclose(file);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t WebServer::send_json_response(httpd_req_t *req, const char *json)
{
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    return ESP_OK;
}

esp_err_t WebServer::send_error_response(httpd_req_t *req, int status, const char *message)
{
    httpd_resp_send_err(req, (httpd_err_code_t)status, message);
    return ESP_OK;
}

std::string WebServer::presets_to_json()
{
    DmxPresets *dmxPresets_ = nullptr; // TODO: Set somewhere
    if (!dmxPresets_)
    {
        return "[]";
    }

    cJSON *root = cJSON_CreateArray();
    if (!root)
    {
        return "[]";
    }

    for (uint8_t i = 0; i < dmxPresets_->getNumPresets(); i++)
    {
        const DmxPreset &preset = dmxPresets_->getPreset(i);

        cJSON *preset_obj = cJSON_CreateObject();
        if (!preset_obj)
            continue;

        cJSON_AddNumberToObject(preset_obj, "index", preset.getIndex());
        cJSON_AddStringToObject(preset_obj, "name", preset.getName());

        // Universe 1
        cJSON *universe1 = cJSON_CreateArray();
        const uint8_t *u1_data = preset.getUniverseData(0);
        for (int j = 0; j < DMX_UNIVERSE_SIZE; j++)
        {
            cJSON_AddItemToArray(universe1, cJSON_CreateNumber(u1_data[j]));
        }
        cJSON_AddItemToObject(preset_obj, "universe1", universe1);

        // Universe 2
        cJSON *universe2 = cJSON_CreateArray();
        const uint8_t *u2_data = preset.getUniverseData(1);
        for (int j = 0; j < DMX_UNIVERSE_SIZE; j++)
        {
            cJSON_AddItemToArray(universe2, cJSON_CreateNumber(u2_data[j]));
        }
        cJSON_AddItemToObject(preset_obj, "universe2", universe2);

        cJSON_AddItemToArray(root, preset_obj);
    }

    char *json_str = cJSON_Print(root);
    std::string result = json_str ? json_str : "[]";
    cJSON_free(json_str);
    cJSON_Delete(root);

    return result;
}

esp_err_t WebServer::json_to_presets(const char *json)
{
    DmxPresets *dmxPresets_ = nullptr; // TODO: Set somewhere
    if (!dmxPresets_ || !json)
    {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(json);
    if (!root || !cJSON_IsArray(root))
    {
        ESP_LOGE(TAG, "Invalid JSON: not an array");
        if (root)
            cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    int num_presets = cJSON_GetArraySize(root);
    if (num_presets < 2 || num_presets > 20)
    {
        ESP_LOGE(TAG, "Invalid number of presets: %d", num_presets);
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    // Set number of presets
    esp_err_t ret = dmxPresets_->setNumPresets(num_presets);
    if (ret != ESP_OK)
    {
        cJSON_Delete(root);
        return ret;
    }

    // Load each preset
    for (int i = 0; i < num_presets; i++)
    {
        cJSON *preset_obj = cJSON_GetArrayItem(root, i);
        if (!preset_obj || !cJSON_IsObject(preset_obj))
        {
            ESP_LOGE(TAG, "Invalid preset object at index %d", i);
            continue;
        }

        DmxPreset preset;

        // Index
        cJSON *index = cJSON_GetObjectItem(preset_obj, "index");
        if (index && cJSON_IsNumber(index))
        {
            preset.setIndex((uint8_t)index->valuedouble);
        }
        // Name
        cJSON *name = cJSON_GetObjectItem(preset_obj, "name");
        if (name && cJSON_IsString(name))
        {
            preset.setName(name->valuestring);
        }

        // Universe 1
        cJSON *universe1 = cJSON_GetObjectItem(preset_obj, "universe1");
        if (universe1 && cJSON_IsArray(universe1))
        {
            uint8_t u1_data[DMX_UNIVERSE_SIZE] = {0};
            int u1_size = cJSON_GetArraySize(universe1);
            int copy_size = u1_size < DMX_UNIVERSE_SIZE ? u1_size : DMX_UNIVERSE_SIZE;
            for (int j = 0; j < copy_size; j++)
            {
                cJSON *val = cJSON_GetArrayItem(universe1, j);
                if (val && cJSON_IsNumber(val))
                {
                    u1_data[j] = (uint8_t)val->valuedouble;
                }
            }
            preset.setUniverseData(0, u1_data, DMX_UNIVERSE_SIZE);
        }

        // Universe 2
        cJSON *universe2 = cJSON_GetObjectItem(preset_obj, "universe2");
        if (universe2 && cJSON_IsArray(universe2))
        {
            uint8_t u2_data[DMX_UNIVERSE_SIZE] = {0};
            int u2_size = cJSON_GetArraySize(universe2);
            int copy_size = u2_size < DMX_UNIVERSE_SIZE ? u2_size : DMX_UNIVERSE_SIZE;
            for (int j = 0; j < copy_size; j++)
            {
                cJSON *val = cJSON_GetArrayItem(universe2, j);
                if (val && cJSON_IsNumber(val))
                {
                    u2_data[j] = (uint8_t)val->valuedouble;
                }
            }
            preset.setUniverseData(1, u2_data, DMX_UNIVERSE_SIZE);
        }

        // Save preset
        dmxPresets_->setPreset(i, preset);
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "Loaded %d presets from JSON", num_presets);
    return ESP_OK;
}

std::string WebServer::config_to_json()
{
    // TODO if (!configurator_)
    {
        return "{}";
    }

    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        return "{}";
    }

    // TODO const char *polarity_str = (footSwitch->getFootSwitchPolarity() == FootSwitch::NORMAL) ? "NORMAL" :
    // "INVERSE";
    // TODO cJSON_AddStringToObject(root, "footSwitchPolarity", polarity_str);
    // TODO cJSON_AddNumberToObject(root, "footSwitchLongPressTime", configurator_->getLongPressTimeMs());

    char *json_str = cJSON_Print(root);
    std::string result = json_str ? json_str : "{}";
    cJSON_free(json_str);
    cJSON_Delete(root);

    return result;
}

esp_err_t WebServer::json_to_config(const char *json, FootSwitch *footSwitch)
{
    if (!footSwitch || !json)
    {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(json);
    if (!root || !cJSON_IsObject(root))
    {
        ESP_LOGE(TAG, "Invalid JSON: not an object");
        if (root)
            cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    // Foot switch polarity
    cJSON *polarity = cJSON_GetObjectItem(root, "footSwitchPolarity");
    if (polarity && cJSON_IsString(polarity))
    {
        // TODO Get foot switch polarity and long press threshold
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "Configuration updated from JSON");
    return ESP_OK;
}
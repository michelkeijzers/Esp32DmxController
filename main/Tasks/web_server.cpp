#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include "web_server.hpp"
#include <esp_log.h>

#include "../Base/assert.hpp"
#include "../ThirdParty/nlohmann/json.hpp"
#include "foot_switch.hpp"
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
        .dmx_values { display: flex; flex-wrap: wrap; gap: 2px; margin: 10px 0; }
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
                    <div class="dmx_values">
                        ${Array.from({length: 512}, (_, i) => `
                            <div class="channel ${preset.dmx_values[i] > 0 ? 'on' : ''}"
                                 style="background-color: rgb(${preset.dmx_values[i]}, ${preset.dmx_values[i]}, ${preset.dmx_values[i]})"
                                 onclick="app.toggleChannel(${index}, ${i})">
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
            dmx_values: new Array(512).fill(0)
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

    toggleChannel(presetIndex, channel) {
        const preset = this.presets[presetIndex];
        const dmxValues = preset.dmx_values;
        dmxValues[channel] = dmxValues[channel] > 0 ? 0 : 255;
        this.render();
    }
}

// Initialize the app
const app = new DMXController();
)js";

WebServer::WebServer(IAssert *assert)
    : RtosTask(), server_(nullptr), initialized_(false), taskHandle_(nullptr), eventQueue_(nullptr), assert_(assert)
{
    instance_ = this;
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

void WebServer::taskLoop()
{
    WebServerEvent event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            // TODO Future message handling can be added here
        }
    }
}

void WebServer::init_spiffs()
{
    esp_vfs_spiffs_conf_t conf;
    conf.base_path = "/spiffs";
    conf.partition_label = NULL;
    conf.max_files = 8;
    conf.format_if_mount_failed = true;
    assert_->assertNotEspError(esp_vfs_spiffs_register(&conf), "Failed to initialize SPIFFS");
    ESP_LOGI(TAG, "SPIFFS initialized");
}

void WebServer::init(RtosTask::TaskProperties taskProperties)
{
    if (initialized_)
    {
        return;
    }

    RtosTask::init(taskProperties);

    // Mount SPIFFS
    init_spiffs();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 8;

    httpd_start(&server_, &config);

    // Register URI handlers
    httpd_uri_t root_uri;
    root_uri.uri = "/";
    root_uri.method = HTTP_GET;
    // TODO: root_uri.handler = root_handler;
    root_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &root_uri);

    httpd_uri_t api_presets_uri;
    api_presets_uri.uri = "/api/presets";
    api_presets_uri.method = HTTP_GET;
    // TODO: api_presets_uri.handler = api_presets_handler;
    api_presets_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &api_presets_uri);

    httpd_uri_t api_presets_post_uri;
    api_presets_post_uri.uri = "/api/presets";
    api_presets_post_uri.method = HTTP_POST;
    // TODO: api_presets_post_uri.handler = api_presets_handler;
    api_presets_post_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &api_presets_post_uri);

    httpd_uri_t api_config_uri;
    api_config_uri.uri = "/api/config";
    api_config_uri.method = HTTP_GET;
    // TODO: api_config_uri.handler = api_config_handler;
    api_config_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &api_config_uri);

    httpd_uri_t api_config_post_uri;
    api_config_post_uri.uri = "/api/config";
    api_config_post_uri.method = HTTP_POST;
    // TODO: api_config_post_uri.handler = api_config_handler;
    api_config_post_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &api_config_post_uri);

    httpd_uri_t api_all_data_get_uri;
    api_all_data_get_uri.uri = "/all_data";
    api_all_data_get_uri.method = HTTP_GET;
    // TODO: api_all_data_get_uri.handler = api_all_data_handler;
    api_all_data_get_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &api_all_data_get_uri);

    httpd_uri_t api_all_data_post_uri;
    api_all_data_post_uri.uri = "/all_data";
    api_all_data_post_uri.method = HTTP_POST;
    // TODO: api_all_data_post_uri.handler = api_all_data_handler;
    api_all_data_post_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &api_all_data_post_uri);

    httpd_uri_t static_file_uri;
    static_file_uri.uri = "/*";
    static_file_uri.method = HTTP_GET;
    // TODO: //TODO: static_file_uri.handler = static_file_handler;
    static_file_uri.user_ctx = nullptr;
    httpd_register_uri_handler(server_, &static_file_uri);

    initialized_ = true;
}

void WebServer::api_all_data_handler(httpd_req_t *req)
{
    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return;
    }

    if (req->method == HTTP_GET)
    {
        // Compose JSON with both configuration and presets
        std::string config_json = instance_->config_to_json();
        std::string presets_json = instance_->presets_to_json();

        nlohmann::json root;
        root["configuration"] = nlohmann::json::parse(config_json);
        root["presets"] = nlohmann::json::parse(presets_json);
        std::string json_str = root.dump();
        instance_->send_json_response(req, json_str.c_str());
        return;
    }
    else if (req->method == HTTP_POST)
    {
        // Parse JSON and update both configuration and presets
        char content[8192];
        int ret = httpd_req_recv(req, content, sizeof(content) - 1);
        if (ret <= 0)
        {
            instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "No data received");
            return;
        }
        content[ret] = '\0';

        nlohmann::json root = nlohmann::json::parse(content);
        if (!root.is_object())
        {
            instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
            return;
        }

        // Update configuration
        if (root.contains("configuration"))
        {
            std::string config_str = root["configuration"].dump();
            instance_->json_to_config(config_str.c_str(), nullptr);
        }

        // Update presets
        if (root.contains("presets"))
        {
            std::string presets_str = root["presets"].dump();
            instance_->json_to_presets(presets_str.c_str());
        }

        instance_->send_json_response(req, "{\"status\":\"ok\"}");
        return;
    }

    instance_->send_error_response(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
}

void WebServer::root_handler(httpd_req_t *req)
{
    assert_->assertNotNull(instance_, "WebServer instance is null");
    assert_->assertNotNull(req, "httpd_req_t is null");

    // Serve index.html from SPIFFS
    const char *filepath = "/spiffs/index.html";
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "index.html not found");
        return;
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
}

void WebServer::api_presets_handler(httpd_req_t *req)
{
    assert_->assertNotNull(req, "req instance is null");

    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return;
    }

    if (req->method == HTTP_GET)
    {
        // Return presets as JSON
        std::string json = instance_->presets_to_json();
        instance_->send_json_response(req, json.c_str());
        return;
    }
    else if (req->method == HTTP_POST)
    {
        // Parse JSON and update presets
        char content[4096];
        int ret = httpd_req_recv(req, content, sizeof(content));
        if (ret <= 0)
        {
            instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "No data received");
            return;
        }
        content[ret] = '\0';

        nlohmann::json root = nlohmann::json::parse(content);
        if (!root.is_object()) {
            instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
            return;
        }

        // Update configuration
        if (root.contains("configuration"))
        {
            std::string config_str = root["configuration"].dump();
            instance_->json_to_config(config_str.c_str(), nullptr);
        }

        // Update presets
        if (root.contains("presets"))
        {
            std::string presets_str = root["presets"].dump();
            instance_->json_to_presets(presets_str.c_str());
        }

        instance_->send_json_response(req, "{\"status\":\"ok\"}");
        return;
    }
}

void WebServer::api_config_handler(httpd_req_t *req)
{
    assert_->assertNotNull(req, "req is NULL");

    ESP_LOGI(TAG, "Received config request: method=%d, uri=%s", req->method, req->uri);
    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return;
    }

    if (req->method == HTTP_GET)
    {
        // Return config as JSON
        std::string json = instance_->config_to_json();
        instance_->send_json_response(req, json.c_str());
        return;
    }
    else if (req->method == HTTP_POST)
    {
        // Parse JSON and update config
        char content[1024];
        int ret = httpd_req_recv(req, content, sizeof(content));
        if (ret <= 0)
        {
            instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "No data received");
            return;
        }
        content[ret] = '\0';

        ESP_LOGI(TAG, "Received config data: %s", content);

        nlohmann::json root = nlohmann::json::parse(content);
        if (!root.is_object())
        {
            instance_->send_error_response(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
            return;
        }

        // Update configuration
        if (root.contains("footSwitchPolarity") && root["footSwitchPolarity"].is_string())
        {
            // TODO Get foot switch polarity and long press threshold
        }

        ESP_LOGI(TAG, "Configuration updated from JSON");
        instance_->send_json_response(req, "{\"status\":\"ok\"}");
        return;
    }

    instance_->send_error_response(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
}

void WebServer::static_file_handler(httpd_req_t *req)
{
    assert_->assertNotNull(req, "req is NULL");

    if (!instance_)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Server not initialized");
        return;
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
            return;
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
    return;
}

void WebServer::send_json_response(httpd_req_t *req, const char *json)
{

    assert_->assertNotNull(req, "req is NULL");
    assert_->assertNotNull(json, "json is NULL");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
}

void WebServer::send_error_response(httpd_req_t *req, int status, const char *message)
{
    assert_->assertNotNull(req, "req is NULL");
    assert_->assertNotNull(message, "message is NULL");

    httpd_resp_send_err(req, (httpd_err_code_t)status, message);
}

std::string WebServer::presets_to_json()
{
    if (!dmxPresets_)
    {
        ESP_LOGE(TAG, "dmxPresets_ is null");
        return "[]";
    }

    nlohmann::json root = nlohmann::json::array();
    uint8_t num_presets = dmxPresets_->getNumberOfFilledPresets();
    ESP_LOGI(TAG, "Number of filled presets: %d", num_presets);

    for (uint8_t i = 0; i < num_presets; i++)
    {
        const DmxPreset &preset = dmxPresets_->getPreset(i);
        ESP_LOGI(TAG, "Processing preset %d: index=%d, name=%s", i, preset.getIndex(), preset.getName());

        nlohmann::json preset_obj;
        preset_obj["index"] = preset.getIndex();
        preset_obj["name"] = preset.getName();

        std::vector<uint8_t> dmx_values(preset.getDmxValues(), preset.getDmxValues() + NR_OF_DMX_CHANNELS);
        preset_obj["dmxValues"] = dmx_values;

        root.push_back(preset_obj);
        ESP_LOGI(TAG, "Added preset %d to JSON array", i);
        ESP_LOGI(TAG, "Preset %d isInitialized: %d", i, preset.isInitialized());
    }

    std::string result = root.dump();
    // ESP_LOGI(TAG, "Generated JSON: %s", result.c_str());
    return result;
}

void WebServer::json_to_presets(const char *json_str)
{
    assert_->assertNotNull(json_str, "json_str is NULL");

    assert_->assertNotNull(dmxPresets_, "dmxPresets_ is null");
    assert_->assertNotNull(json_str, "json_str is null");

    nlohmann::json root = nlohmann::json::parse(json_str);
    assert_->assertTrue(root.is_array(), "Invalid JSON: not an array");

    size_t num_presets = root.size();
    assert_->assertTrue(num_presets >= 2 && num_presets <= 20, "Invalid number of presets");

    // Set number of presets
    dmxPresets_->setNumberOfFilledPresets(static_cast<int>(num_presets));

    // Load each preset
    for (size_t i = 0; i < num_presets; i++)
    {
        assert_->assertTrue(i < root.size(), "Preset is missing in JSON");
        DmxPreset preset(assert_);

        // Index
        if (root[i].contains("index") && root[i]["index"].is_number())
            preset.setIndex((uint8_t)root[i]["index"]);
        // Name
        if (root[i].contains("name") && root[i]["name"].is_string())
        {
            std::string name_str = root[i]["name"].get<std::string>();
            preset.setName(name_str.c_str());
        }

        // DMX Values
        if (root[i].contains("dmxValues") && root[i]["dmxValues"].is_array())
        {
            uint8_t dmx_values[NR_OF_DMX_CHANNELS] = {0};
            size_t dmx_values_size = root[i]["dmxValues"].size();
            size_t copy_size = dmx_values_size < NR_OF_DMX_CHANNELS ? dmx_values_size : NR_OF_DMX_CHANNELS;
            for (size_t j = 0; j < copy_size; j++)
            {
                if (root[i]["dmxValues"][j].is_number())
                    dmx_values[j] = (uint8_t)root[i]["dmxValues"][j];
            }
            preset.setDmxValues(dmx_values);
        }

        // Save preset
        dmxPresets_->setPreset(static_cast<uint8_t>(i), preset);
    }

    ESP_LOGI(TAG, "Loaded %zu presets from JSON", num_presets);
}

std::string WebServer::config_to_json()
{
    nlohmann::json root;
    // TODO: Add actual config fields
    // root["footSwitchPolarity"] = ...;
    // root["footSwitchLongPressTime"] = ...;
    return root.dump();
}

void WebServer::json_to_config(const char *json_str, FootSwitch *footSwitch)
{
    assert_->assertNotNull(json_str, "json_str is null");
    assert_->assertNotNull(footSwitch, "footSwitch is null");

    nlohmann::json root = nlohmann::json::parse(json_str);
    assert_->assertTrue(root.is_object(), "Invalid JSON: not an object");

    // Foot switch polarity
    if (root.contains("footSwitchPolarity") && root["footSwitchPolarity"].is_string())
    {
        // TODO Get foot switch polarity and long press threshold
    }

    ESP_LOGI(TAG, "Configuration updated from JSON");
}

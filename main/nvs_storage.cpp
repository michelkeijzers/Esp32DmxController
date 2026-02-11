#include "nvs_storage.hpp"
#include <esp_log.h>
#include <cstring>

static const char *TAG = "NVSStorage";

NVSStorage::NVSStorage(const char *ns) : nvs_handle(0), namespace_name(ns),
                                         taskHandle_(nullptr), eventQueue_(nullptr)
{
    eventQueue_ = xQueueCreate(4, sizeof(NvsEvent));
    if (eventQueue_)
    {
        xTaskCreate(taskEntry, "NVSStorageTask", 4096, this, 5, &taskHandle_);
        ESP_LOGI(TAG, "NVSStorage task started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create NVSStorage event queue");
    }
}

NVSStorage::~NVSStorage()
{
    if (taskHandle_)
    {
        vTaskDelete(taskHandle_);
    }
    if (eventQueue_)
    {
        vQueueDelete(eventQueue_);
    }
    if (nvs_handle != 0)
    {
        nvs_close(nvs_handle);
    }
}

void NVSStorage::postEvent(const NvsEvent &event)
{
    if (eventQueue_)
    {
        xQueueSend(eventQueue_, &event, 0);
    }
}

void NVSStorage::taskEntry(void *param)
{
    static_cast<NVSStorage *>(param)->taskLoop();
}

void NVSStorage::taskLoop()
{
    NvsEvent event;
    while (true)
    {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE)
        {
            // Implement event handling logic here as needed
            // For now, just log the event type
            ESP_LOGI(TAG, "NVSStorage event received: %d", event.type);
        }
    }
}

esp_err_t NVSStorage::init()
{
    esp_err_t err = nvs_open(namespace_name, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSStorage::storeString(const char *key, const char *value)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    esp_err_t err = nvs_set_str(nvs_handle, key, value);
    if (err == ESP_OK)
    {
        err = nvs_commit(nvs_handle);
    }
    return err;
}

esp_err_t NVSStorage::getString(const char *key, char *buffer, size_t buffer_size)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    size_t required_size;
    esp_err_t err = nvs_get_str(nvs_handle, key, NULL, &required_size);
    if (err != ESP_OK)
        return err;

    if (required_size > buffer_size)
    {
        return ESP_ERR_INVALID_SIZE;
    }

    return nvs_get_str(nvs_handle, key, buffer, &required_size);
}

esp_err_t NVSStorage::storeInt(const char *key, int32_t value)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    esp_err_t err = nvs_set_i32(nvs_handle, key, value);
    if (err == ESP_OK)
    {
        err = nvs_commit(nvs_handle);
    }
    return err;
}

esp_err_t NVSStorage::getInt(const char *key, int32_t *value)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;
    return nvs_get_i32(nvs_handle, key, value);
}

esp_err_t NVSStorage::storeBlob(const char *key, const void *data, size_t length)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    esp_err_t err = nvs_set_blob(nvs_handle, key, data, length);
    if (err == ESP_OK)
    {
        err = nvs_commit(nvs_handle);
    }
    return err;
}

esp_err_t NVSStorage::getBlob(const char *key, void *buffer, size_t buffer_size, size_t *length)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    esp_err_t err = nvs_get_blob(nvs_handle, key, NULL, length);
    if (err != ESP_OK)
        return err;

    if (*length > buffer_size)
    {
        return ESP_ERR_INVALID_SIZE;
    }

    return nvs_get_blob(nvs_handle, key, buffer, length);
}

esp_err_t NVSStorage::eraseKey(const char *key)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    esp_err_t err = nvs_erase_key(nvs_handle, key);
    if (err == ESP_OK)
    {
        err = nvs_commit(nvs_handle);
    }
    return err;
}

esp_err_t NVSStorage::eraseAll()
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    esp_err_t err = nvs_erase_all(nvs_handle);
    if (err == ESP_OK)
    {
        err = nvs_commit(nvs_handle);
    }
    return err;
}

esp_err_t NVSStorage::keyExists(const char *key, bool *exists)
{
    if (!nvs_handle)
        return ESP_ERR_INVALID_STATE;

    *exists = false;
    size_t required_size = 0;
    esp_err_t err = nvs_get_str(nvs_handle, key, NULL, &required_size);
    if (err == ESP_OK)
    {
        *exists = true;
    }
    else if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        err = ESP_OK; // Key doesn't exist, but that's not an error
    }
    return err;
}

esp_err_t NVSStorage::getStats(nvs_stats_t *stats)
{
    return nvs_get_stats(NULL, stats);
}
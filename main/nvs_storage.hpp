#ifndef NVS_STORAGE_HPP
#define NVS_STORAGE_HPP

#include <nvs.h>
#include <esp_err.h>
#include <string>
#include <vector>

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
}

class NVSStorage
{
public:
    enum EventType
    {
        STORE_STRING,
        GET_STRING,
        STORE_INT,
        GET_INT,
        STORE_BLOB,
        GET_BLOB,
        ERASE_KEY,
        ERASE_ALL,
        KEY_EXISTS,
        GET_STATS
    };

    struct NvsEvent
    {
        EventType type;
        void *arg1;
        void *arg2;
        void *arg3;
        void *result;
    };

    NVSStorage(const char *ns = "dmx_ctrl");
    ~NVSStorage();

    esp_err_t init();

    // Post an event to the NVS task
    void postEvent(const NvsEvent &event);

    // Synchronous wrappers (for compatibility)
    esp_err_t storeString(const char *key, const char *value);
    esp_err_t getString(const char *key, char *buffer, size_t buffer_size);
    esp_err_t storeInt(const char *key, int32_t value);
    esp_err_t getInt(const char *key, int32_t *value);
    esp_err_t storeBlob(const char *key, const void *data, size_t length);
    esp_err_t getBlob(const char *key, void *buffer, size_t buffer_size, size_t *length);
    esp_err_t eraseKey(const char *key);
    esp_err_t eraseAll();
    esp_err_t keyExists(const char *key, bool *exists);
    esp_err_t getStats(nvs_stats_t *stats);

private:
    nvs_handle_t nvs_handle;
    const char *namespace_name;
    TaskHandle_t taskHandle_;
    QueueHandle_t eventQueue_;

    static void taskEntry(void *param);
    void taskLoop();
};

#endif // NVS_STORAGE_HPP
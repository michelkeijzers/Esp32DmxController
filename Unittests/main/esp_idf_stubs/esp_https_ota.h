#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for ESP-IDF esp_https_ota.h

#define ESP_OK 0
#define ESP_FAIL -1

    typedef void *esp_https_ota_handle_t;
    typedef struct
    {
        int dummy;
    } esp_https_ota_config_t;
    static inline int esp_https_ota(const esp_https_ota_config_t *) { return 0; }

#ifdef __cplusplus
}
#endif

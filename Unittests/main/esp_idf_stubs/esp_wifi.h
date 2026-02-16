#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for ESP-IDF esp_wifi.h

#define ESP_OK 0
#define ESP_FAIL -1

    typedef void *wifi_config_t;
    typedef void *wifi_init_config_t;

    typedef enum
    {
        WIFI_MODE_NULL = 0,
        WIFI_MODE_STA,
        WIFI_MODE_AP,
        WIFI_MODE_APSTA
    } wifi_mode_t;

#ifdef __cplusplus
}
#endif

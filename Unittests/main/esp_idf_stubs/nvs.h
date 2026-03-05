#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for ESP-IDF nvs.h

#include <stddef.h>
    typedef int nvs_handle_t;
    typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define NVS_READWRITE 1

    static inline esp_err_t nvs_open(const char *, int, nvs_handle_t *) { return ESP_OK; }
    static inline esp_err_t nvs_close(nvs_handle_t) { return ESP_OK; }
    static inline esp_err_t nvs_set_u8(nvs_handle_t, const char *, unsigned char) { return ESP_OK; }
    static inline esp_err_t nvs_set_u16(nvs_handle_t, const char *, unsigned short) { return ESP_OK; }
    static inline esp_err_t nvs_commit(nvs_handle_t) { return ESP_OK; }
    static inline esp_err_t nvs_get_u8(nvs_handle_t, const char *, unsigned char *) { return ESP_OK; }
    static inline esp_err_t nvs_get_u16(nvs_handle_t, const char *, unsigned short *) { return ESP_OK; }

    static inline esp_err_t nvs_set_blob(nvs_handle_t, const char *, const void *, size_t) { return ESP_OK; }
    static inline esp_err_t nvs_get_blob(nvs_handle_t, const char *, void *, size_t *) { return ESP_OK; }

    static inline esp_err_t nvs_get_str(nvs_handle_t, const char *, char *, size_t *) { return ESP_OK; }

#ifdef __cplusplus
}
#endif

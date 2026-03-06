#include "nvs.h"

static esp_err_t default_get_u8(nvs_handle_t, const char *, unsigned char *) { return ESP_OK; }
static esp_err_t default_get_str(nvs_handle_t, const char *, char *, size_t *) { return ESP_OK; }
static esp_err_t default_get_blob(nvs_handle_t, const char *, void *, size_t *) { return ESP_OK; }

esp_err_t (*nvs_get_u8_ptr)(nvs_handle_t, const char *, unsigned char *) = default_get_u8;
esp_err_t (*nvs_get_str_ptr)(nvs_handle_t, const char *, char *, size_t *) = default_get_str;
esp_err_t (*nvs_get_blob_ptr)(nvs_handle_t, const char *, void *, size_t *) = default_get_blob;

extern "C"
{
    esp_err_t nvs_get_u8(nvs_handle_t h, const char *k, unsigned char *v) { return nvs_get_u8_ptr(h, k, v); }
    esp_err_t nvs_get_str(nvs_handle_t h, const char *k, char *v, size_t *l) { return nvs_get_str_ptr(h, k, v, l); }
    esp_err_t nvs_get_blob(nvs_handle_t h, const char *k, void *v, size_t *l) { return nvs_get_blob_ptr(h, k, v, l); }
}

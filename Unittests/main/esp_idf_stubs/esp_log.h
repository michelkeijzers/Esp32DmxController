#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for ESP-IDF esp_log.h

#include <stdio.h>
#define ESP_LOGI(tag, fmt, ...) fprintf(stdout, "[I] %s: " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) fprintf(stderr, "[W] %s: " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, fmt, ...) fprintf(stderr, "[E] %s: " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) fprintf(stdout, "[D] %s: " fmt, tag, ##__VA_ARGS__)
#define ESP_LOG_BUFFER_HEXDUMP(tag, buffer, buff_len, level) ((void)0)

#ifdef __cplusplus
}
#endif

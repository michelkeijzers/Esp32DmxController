#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
// Minimal stub for esp_spiffs.h
#ifdef __cplusplus
}
#endif

#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for esp_spiffs.h

    typedef struct
    {
        const char *base_path;
        const char *partition_label;
        size_t max_files;
        bool format_if_mount_failed;
    } esp_vfs_spiffs_conf_t;

    typedef int esp_err_t;
    static inline int esp_vfs_spiffs_register(const esp_vfs_spiffs_conf_t *) { return 0; }

#ifdef __cplusplus
}
#endif

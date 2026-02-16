#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for esp_app_desc.h

    typedef struct esp_app_desc
    {
        char version[32];
        char project_name[32];
        char date[16];
        char time[16];
    } esp_app_desc_t;

    static inline const esp_app_desc_t *esp_app_get_description(void)
    {
        static esp_app_desc_t desc = {0};
        return &desc;
    }

#ifdef __cplusplus
}
#endif

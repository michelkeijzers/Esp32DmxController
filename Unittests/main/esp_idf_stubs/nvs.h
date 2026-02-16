#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    // Minimal stub for ESP-IDF nvs.h

    typedef void *nvs_handle_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define NVS_READWRITE 1

    static inline int nvs_open(const char *, int, void **) { return 0; }
    static inline int nvs_close(void *) { return 0; }
    static inline int nvs_set_u8(void *, const char *, unsigned char) { return 0; }
    static inline int nvs_set_u16(void *, const char *, unsigned short) { return 0; }
    static inline int nvs_commit(void *) { return 0; }
    static inline int nvs_get_u8(void *, const char *, unsigned char *) { return 0; }
    static inline int nvs_get_u16(void *, const char *, unsigned short *) { return 0; }
    static inline int nvs_set_blob(void *, const char *, const void *, size_t) { return 0; }
    static inline int nvs_get_blob(void *, const char *, void *, size_t *) { return 0; }

#ifdef __cplusplus
}
#endif

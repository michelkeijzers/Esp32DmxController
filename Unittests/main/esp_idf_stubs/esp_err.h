#ifdef __cplusplus
extern "C"
{
#endif

    typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERROR_CHECK(x) (void)(x)
#define ESP_ERR_INVALID_ARG -2
#define ESP_ERR_INVALID_STATE -3

    static inline const char *esp_err_to_name(esp_err_t err)
    {
        switch (err)
        {
        case ESP_OK:
            return "ESP_OK";
        case ESP_FAIL:
            return "ESP_FAIL";
        case ESP_ERR_INVALID_ARG:
            return "ESP_ERR_INVALID_ARG";
        case ESP_ERR_INVALID_STATE:
            return "ESP_ERR_INVALID_STATE";
        default:
            return "ESP_ERR_UNKNOWN";
        }
    }

#ifdef __cplusplus
}
#endif
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERROR_CHECK(x) (void)(x)
#define ESP_ERR_INVALID_ARG -2
#define ESP_ERR_INVALID_STATE -3

#ifdef __cplusplus
}
#endif
